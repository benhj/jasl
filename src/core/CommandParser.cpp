//
//  CommandParser.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2017-present Ben Jones. All rights reserved.
//

#include "CommandParser.hpp"
#include "parser/Escapes.hpp"
#include "parser/Strings.hpp"

namespace jasl
{
    CommandParser::CommandParser() : CommandParser::base_type(start)
    {
        // token types that we're interested in using
        using qi::long_long;
        using qi::lit;
        using qi::eol;
        using qi::double_;
        using qi::bool_;
        using qi::lexeme;
        using qi::raw;
        using ascii::char_;
        using ascii::string;
        using ascii::blank;
        using qi::uint_;

        // convenience rules
        Escapes::init();
        Strings::init();

        allChars            %= lexeme[+(char_ - '\n')];
        commentSlash        %= string("//");
        commentColons       %= string(";;;");
        commentPound        %= string("#");
        commentFunc         %= (commentColons | commentSlash | commentPound) >> -allChars;
        word                %= lexeme[+(char_ - ' ' - ',' - '[' - ']' - '(' - ')' - '^' - '?')];
        carrotWord          %= lexeme['^' >> +(char_ - ' ' - ',' - '[' - ']' - '(' - ')' - '^')];

        // symbols to be used in a math expression
        mathSymbols %= lexeme[ string("%")
                             | string("+")
                             | string("-")
                             | string("/")
                             | string("*")
                             | string("^")];

        // logical symbols
        comparisonSymbols %= lexeme[ string("==")
                                   | string(">=")
                                   | string(">")
                                   | string("<=")
                                   | string("<")
                                   | string("&&")
                                   | string("||")
                                   | string("!=")
                                   | string("=")
                                   | string("/=")];

        // a math expression enclose by brackets to aid compound expressions
        // e.g. (5.0 + 5.0)
        bracketedMathExpression %= '('
                                >> (doubleRule | intRule | Strings::genericString | bracketedMathExpression)
                                >> mathSymbols
                                >> (doubleRule | intRule | Strings::genericString | bracketedMathExpression)
                                >> ')';

        // a simple math expression e.g. 5 + 5
        mathExpression %= (doubleRule | intRule | Strings::genericString | bracketedMathExpression)
                           >> mathSymbols
                           >> (doubleRule | intRule | Strings::genericString | bracketedMathExpression);

        // a logical expression inside a pair of brackets e.g. (2 > 1)
        bracketedComparisonExpression  %= '('
                                       >> ( bracketedComparisonExpression
                                          | bracketedMathExpression
                                          | doubleRule
                                          | intRule
                                          | Strings::genericString
                                          | boolRule)
                                       >> comparisonSymbols
                                       >> ( bracketedComparisonExpression
                                          | bracketedMathExpression
                                          | doubleRule
                                          | intRule
                                          | Strings::genericString
                                          | boolRule)
                                       >> ')';

        // a simple logical expression e.g 2 > 1
        comparisonExpression %= ( doubleRule
                                | intRule
                                | Strings::genericString
                                | boolRule
                                | bracketedMathExpression
                                | bracketedComparisonExpression)
                             >> comparisonSymbols
                             >> ( doubleRule
                                | intRule
                                | Strings::genericString
                                | boolRule
                                | bracketedMathExpression
                                | bracketedComparisonExpression);

        arrayTypes %= lexeme[string("int") | string("real") | string("byte") | string("string")];

        // simpler array types. Arrays are 'plural' versions of primitives
        // reals 5 -> a;
        reals %= string("reals")
              >> (('('
              >> (mathExpression | bracketedMathExpression | intRule | Strings::genericString)
              >> ')') | Strings::genericString)
              >> lit("->")
              >> (Strings::genericString)
              >> ';';

        ints %= string("ints")
              >> (('('
              >> (mathExpression | bracketedMathExpression | intRule | Strings::genericString)
              >> ')') | Strings::genericString)
              >> lit("->")
              >> (Strings::genericString)
              >> ';';

        bytes %= string("bytes")
              >> (('('
              >> (mathExpression | bracketedMathExpression | intRule | Strings::genericString)
              >> ')') | Strings::genericString)
              >> lit("->")
              >> (Strings::genericString)
              >> ';';

        bools %= string("bools")
              >> (('('
              >> (mathExpression | bracketedMathExpression | intRule | Strings::genericString)
              >> ')') | Strings::genericString)
              >> lit("->")
              >> (Strings::genericString)
              >> ';';

        strings %= string("strings")
              >> (('('
              >> (mathExpression | bracketedMathExpression | intRule | Strings::genericString)
              >> ')') | Strings::genericString)
              >> lit("->")
              >> (Strings::genericString)
              >> ';';

        //
        // with thanks to sehe and llonesmix @
        // http://stackoverflow.com/questions/15212671/
        // using-boostspirit-to-parse-multiple-types-of-single-value
        // #comment21459703_15212671
        //
        using RealPolicy = boost::spirit::qi::strict_real_policies<double>;
        using RealParse = boost::spirit::qi::real_parser<double, RealPolicy>;
        RealParse double_with_point;
        doubleRule %= (long_long >> 'f')
                    | (double_with_point >> -lit('f'));
        intRule %= long_long;
        boolRule %= bool_;

        brackets %= "()";

        // represents a pair of values e.g. (5, 5)
        pairRule %= '(' >> (doubleRule | intRule | Strings::genericString) >> ','
                        >> (doubleRule | intRule | Strings::genericString) >> ')';

        // respresnts a triple of values e.g. (1, 2, 3)
        tupleRule %= '(' >> (doubleRule | intRule | Strings::genericString) >> ','
                         >> (doubleRule | intRule | Strings::genericString) >> ','
                         >> (doubleRule | intRule | Strings::genericString) >> ')';
    
        // prints out the value of a given variable
        query %= string("query") >> '('
              >> Strings::genericString
               >> ')' >> ';';

        // the entire set of instructions at out disposal
        commandCollection %= *allCommands;

        // a for-loop for iterating over elements in a list
        // e.g. for i in list {...}
        forLoop %= string("for")
                >> Strings::genericString // variable e.g. c
                >> lit("in")
                >> (Strings::genericString | bracketedWords) 
                >> '{'
                >>  commandCollection
                >> '}';

        // for a loop having the syntax
        // repeat N times {  }
        repeatLoop %= string("repeat") 
                    >> (intRule | Strings::genericString | bracketedMathExpression)
                    >> lit("times")
                    >> '{'
                    >> commandCollection
                    >> '}';

        // for a loop having the syntax
        // while (condition) {}
        whileLoop %= string("while") 
                  >> '(' 
                  >> (boolRule | comparisonExpression | bracketedComparisonExpression)
                  >> ')'
                  >> '{'
                  >> commandCollection
                  >> '}';

        // Executes a system command having the syntax
        // exec("/path/to/command"); 
        // or
        // exec(s); 
        // where s is a string
        execCommand %= string("exec") 
                    >> '(' 
                    >> (Strings::doubleQuotedString | Strings::genericString)
                    >> ')'
                    >> ';';

        // the main entry point of a given program
        startFunction %= string("start")
                     >> '{'
                     >>  commandCollection
                     >> '}';

        // Rules defining parameter sets for functions
        parameter      %= (Strings::doubleQuotedString | Strings::genericString | doubleRule | intRule | boolRule | 
                           mathExpression | bracketedMathExpression | 
                           comparisonExpression | bracketedComparisonExpression | bracketedWords);
        commaParameter %= ',' >> parameter;
        barString      %= '|' >> Strings::genericString;
        parameters     %= -(parameter >> *commaParameter); // comma-separated
        concats        %= (Strings::genericString >> *barString); // bar-separated
        // a collection of parameters
        // to be used by a function. This could be empty as in
        // () or have arguments, as in (a, b, c);
        parameterList %= parameters;

        // for string concatenation, use bar. As in (a|b|c)
        concatList    %= '(' >> concats >> ')';

        // a callable execution point
        block %= string("block")
              >> Strings::genericString // functionName
              // take an optional list of arguments
              >> '(' >> parameterList >> ')'
              >> '{'
              >>  commandCollection
              >> '}';

        arrayLexeme     %= lexeme[string("array")];
        returnableArray %= string("fn")
                        >> ':'
                        >> arrayLexeme
                        >> ':' 
                        >> arrayTypes// return type
                        >> Strings::genericString // functionName
                        >> '(' >> parameterList >> ')' // list of parameters
                        >> -(lit("->") >> Strings::genericString)
                        >> '{'
                        >> commandCollection
                        >> '}';

        // a returnable function
        // E.g. a function that returns an int
        // fn:int func() -> result {}
        // or an array:
        // fn:array:real func() -> result {}
        returnable %= string("fn")
                   >> ':'
                   >> Strings::genericString // return type
                   >> Strings::genericString // functionName
                   >> '(' >> parameterList >> ')' // list of parameters
                   >> -(lit("->") >> Strings::genericString)
                   >> '{'
                   >> commandCollection
                   >> '}';

        // a simple if statement
        ifRule %= string("if")
               >> '(' 
               >>  comparisonExpression
               >> ')'
               >> '{'
               >>  commandCollection
               >> '}'
               // optional 'else part'
               >> -(lit("else") >> '{' >> commandCollection >> '}');

        ifRule_B %= string("?")
               >> '(' 
               >>  comparisonExpression
               >> ')'
               >> '{'
               >>  commandCollection
               >> '}'
               // optional 'else part'
               >> -(lit(":") >> '{' >> commandCollection >> '}');

        // calls a function with given name
        call %= string("call")
             >> Strings::genericString // functionName
             >> '(' >> parameterList >> ')' // optional parameters
             // optional return part
             >> -(lit("->") >> Strings::genericString)
             >> ';';

        //  for setting a value in an array
        //  put 5 -> a(0);             
        put  %= string("put")
             >> (Strings::genericString | doubleRule | intRule | mathExpression | bracketedMathExpression)
             >> lit("->")
             >> Strings::genericString 
             // optional indexing
             >> -('('
             >> (mathExpression | bracketedMathExpression | intRule | Strings::genericString)
             >> ')')
             >> ';';

        // for getting a value in an array
        // get a(0) -> val;
        get  %= string("get")
             >> (Strings::genericString)
             >> '('
             >> (mathExpression | bracketedMathExpression | intRule | Strings::genericString)
             >> ')'
             >> lit("->")
             >> (Strings::genericString)
             >> ';';

        // a list of words
        // e.g. list [hello there] -> L;
        // Should also be possible to have lists of lists
        // e.g. list [hello [nested list] there] -> L;
        // will be useful for pattern matching
        // a collection of words
        words          %= *(word | bracketedWords); // zero or more words
        bracketedWords %= '[' >> words >> ']';

        // index_of ("hello", [hello there]) -> s;
        listTokenIndex  %= string("index_of")
                        >> '('
                        >> (Strings::genericString | Strings::doubleQuotedString)
                        >> lit(",")
                        >> (bracketedWords | Strings::genericString)
                        >> (')')
                        >> lit("->")
                        >> Strings::genericString 
                        >> ';';

        // get_token(0, [hello there]) -> t;
        listGetToken  %= string("get_token")
                      >> '('
                      >> (Strings::genericString | intRule)
                      >> ','
                      >> (bracketedWords | Strings::genericString)
                      >> ')'
                      >> lit("->")
                      >> Strings::genericString 
                      >> ';';

        // add_token("token", L);
        listAddToken  %= string("add_token")
                      >> '('
                      >> (Strings::genericString | Strings::doubleQuotedString | bracketedWords)
                      >> ','
                      >> (Strings::genericString)
                      >> ')'
                      >> ';';


        // set_token (0, [hello there], "goodbye") -> t;
        // set_token (0, [hello there], [nested bit]) -> t;
        listSetToken  %= string("set_token")
                      >> ('(')
                      >> (Strings::genericString | intRule)
                      >> (',')
                      >> (bracketedWords | Strings::genericString)
                      >> (',')
                      >> (Strings::genericString | Strings::doubleQuotedString | bracketedWords)
                      >> (')')
                      >> lit("->")
                      >> Strings::genericString 
                      >> ';';

        matchesCommand %= string("matches")
                       >> bracketedWords
                       >> '|'
                       >> bracketedWords
                       >> lit("->")
                       >> Strings::genericString 
                       >> ';';

        // appends to end of a string, s  
        // append (s, "hello") -> result;
        appendRule %= string("append")
                   >> ('(')
                   >> (Strings::doubleQuotedString | Strings::genericString)
                   >> ','
                   >> (Strings::doubleQuotedString | Strings::genericString | doubleRule | intRule | boolRule | 
                       mathExpression | bracketedMathExpression | 
                       comparisonExpression | bracketedComparisonExpression)
                   >> ')'
                   >> lit("->")
                   >> Strings::genericString
                   >> ';';

        // string_reverse name; 
        reverseRule %= string("string_reverse")
                    >> (Strings::doubleQuotedString | Strings::genericString)
                    >> ';';

        // for printing out a string to screen
        pr %= string("pr")
             >> (Strings::doubleQuotedString | Strings::genericString | doubleRule | intRule | boolRule | 
                 mathExpression | bracketedMathExpression | 
                 comparisonExpression | bracketedComparisonExpression)
             >> ';';

        // for printing out a string to screen with newline
        prn %= string("prn")
               >> (Strings::doubleQuotedString | Strings::genericString | doubleRule | intRule | boolRule | 
                   mathExpression | bracketedMathExpression | 
                   comparisonExpression | bracketedComparisonExpression)
               >> ';';

        // equivalent to 'pr' -- slightly nicer syntax
        say %= string("say")
               >> (Strings::doubleQuotedString | Strings::genericString | doubleRule | intRule | boolRule | 
                   mathExpression | bracketedMathExpression | 
                   comparisonExpression | bracketedComparisonExpression)
               >> ';';

        // lists all variables
        vars %= string("vars") >> brackets >> ';';

        // loads a jasl script
        loadScript %= string("script") 
                   >> Strings::doubleQuotedString >> ';';

        // erases a variable from the cache
        releaseCommand %= string("release")
                       >> Strings::genericString >> ';';

        // generates an int or real in somce range [0, n]
        // random:int n -> value;
        // random:real n -> value;
        randomCommand %= string("random") >> ':'
                      >> Strings::genericString
                      >> (Strings::genericString | doubleRule | intRule | 
                           mathExpression | bracketedMathExpression)
                      >> lit("->") 
                      >> Strings::genericString
                      >> ';';

        // for exiting a program
        exitCommand %= string("exit") >> -(Strings::genericString) >> ';';

        regexEqCommand %= string("regex_eq")
                       >> ('(') 
                       >> (Strings::doubleQuotedString | Strings::genericString) >> ','
                       >> (Strings::doubleQuotedString | Strings::genericString) 
                       >> (')')
                       >> lit("->")
                       >> Strings::genericString
                       >> ';';

        regexParseCommand %= string("regex_parse")
                          >> ('(') 
                          >> (Strings::doubleQuotedString | Strings::genericString) >> ','
                          >> (Strings::doubleQuotedString | Strings::genericString) 
                          >> (')')
                          >> lit("->")
                          >> Strings::genericString
                          >> ';';

        wildcardParseCommand %= string("wildcard_parse")
                             >> ('(') 
                             >> (Strings::doubleQuotedString | Strings::genericString) >> ','
                             >> (Strings::doubleQuotedString | Strings::genericString) 
                             >> (')')
                             >> lit("->")
                             >> Strings::genericString
                             >> ';';

        wildcardEqCommand %= string("wildcard_eq")
                          >> ('(') 
                          >> (Strings::doubleQuotedString | Strings::genericString) >> ','
                          >> (Strings::doubleQuotedString | Strings::genericString) 
                          >> (')')
                          >> lit("->")
                          >> Strings::genericString
                          >> ';';
        // concatenate strings. 
        concatRule %= string("concat")
                   >> (parameterList) >> lit("->")
                   >> Strings::genericString 
                   >> ';';

        // matches this type of command:
        // net_send something -> somethingElse;
        genericArrowRule %= Strings::genericString
                         >> (Strings::doubleQuotedString
                             | boolRule
                             | Strings::genericString
                             | doubleRule
                             | intRule
                             | mathExpression
                             | bracketedMathExpression
                             | comparisonExpression
                             | bracketedComparisonExpression
                             | bracketedWords)
                          >> lit("->")
                          >> (Strings::doubleQuotedString | Strings::genericString) 
                          >> ';';

        // matches this type of command:
        // net_send something;
        ansiUPRule %= string("ansi_up")
                            >> (Strings::genericString
                               | intRule
                               | mathExpression
                               | bracketedMathExpression) >> ';';

        // all the instructions at out disposal
        allCommands %= forLoop
                     | query 
                     | startFunction 
                     | block
                     | call 
                     | returnable
                     | returnableArray
                     | put
                     | ints | bytes | strings | bools | reals
                     | prn
                     | pr
                     | say
                     | ifRule 
                     | ifRule_B
                     | commentFunc
                     | vars 
                     | loadScript
                     | repeatLoop
                     | whileLoop
                     | appendRule
                     | reverseRule 
                     | listGetToken
                     | listSetToken
                     | listAddToken
                     | listTokenIndex
                     | execCommand
                     | releaseCommand
                     | randomCommand
                     | exitCommand
                     | get
                     | regexEqCommand
                     | regexParseCommand
                     | wildcardEqCommand
                     | wildcardParseCommand
                     | concatRule
                     | ansiUPRule
                     | genericArrowRule;
                     
        start %= allCommands;
    }
}
