//
//  CommandParser.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2017-present Ben Jones. All rights reserved.
//

#include "CommandParser.hpp"

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
        escapes = '\\' > ( escaped_character[ cp2utf8( ::boost::spirit::_val, ::boost::spirit::_1 ) ]
                                 | ( "x" > qi::uint_parser< UChar32, 16, 2, 2 >()[ cp2utf8( ::boost::spirit::_val, ::boost::spirit::_1 ) ] )
                                 | ( "u" > qi::uint_parser< UChar32, 16, 4, 4 >()[ cp2utf8( ::boost::spirit::_val, ::boost::spirit::_1 ) ] )
                                 | ( "U" > qi::uint_parser< UChar32, 16, 8, 8 >()[ cp2utf8( ::boost::spirit::_val, ::boost::spirit::_1 ) ] )
                                 | qi::uint_parser< UChar32,  8, 1, 3 >()[ cp2utf8( ::boost::spirit::_val, ::boost::spirit::_1 ) ]
                                 );

        escaped_character.add
        (  "a", 0x07 ) // alert
        (  "b", 0x08 ) // backspace
        (  "f", 0x0c ) // form feed
        (  "n", 0x0a ) // new line
        (  "r", 0x0d ) // carriage return
        (  "t", 0x09 ) // horizontal tab
        (  "v", 0x0b ) // vertical tab
        ( "\"", 0x22 ) // literal quotation mark
        ( "\\", 0x5c ) // literal backslash
        ;

        escChar %= '\\' >> char_("n"); // new line
        quotedString        %= lexeme['\''>> *( +( char_ - ( '"' | eol | '\\' ) ) | escapes ) >> '\''];
        doubleQuotedString  %= lexeme['"' >> *( +( char_ - ( '"' | eol | '\\' ) ) | escapes ) >> '"'];
        genericString       %= lexeme[+(char_("a-zA-Z_"))];
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
                                >> (doubleRule | intRule | genericString | bracketedMathExpression)
                                >> mathSymbols
                                >> (doubleRule | intRule | genericString | bracketedMathExpression)
                                >> ')';

        // a simple math expression e.g. 5 + 5
        mathExpression %= (doubleRule | intRule | genericString | bracketedMathExpression)
                           >> mathSymbols
                           >> (doubleRule | intRule | genericString | bracketedMathExpression);

        // a logical expression inside a pair of brackets e.g. (2 > 1)
        bracketedComparisonExpression  %= '('
                                       >> ( bracketedComparisonExpression
                                          | bracketedMathExpression
                                          | doubleRule
                                          | intRule
                                          | genericString
                                          | boolRule)
                                       >> comparisonSymbols
                                       >> ( bracketedComparisonExpression
                                          | bracketedMathExpression
                                          | doubleRule
                                          | intRule
                                          | genericString
                                          | boolRule)
                                       >> ')';

        // a simple logical expression e.g 2 > 1
        comparisonExpression %= ( doubleRule
                                | intRule
                                | genericString
                                | boolRule
                                | bracketedMathExpression
                                | bracketedComparisonExpression)
                             >> comparisonSymbols
                             >> ( doubleRule
                                | intRule
                                | genericString
                                | boolRule
                                | bracketedMathExpression
                                | bracketedComparisonExpression);

        arrayTypes %= lexeme[string("int") | string("real") | string("byte") | string("string")];

        // simpler array types. Arrays are 'plural' versions of primitives
        // reals 5 -> a;
        reals %= string("reals")
              >> (('('
              >> (mathExpression | bracketedMathExpression | intRule | genericString)
              >> ')') | genericString)
              >> lit("->")
              >> (genericString)
              >> ';';

        ints %= string("ints")
              >> (('('
              >> (mathExpression | bracketedMathExpression | intRule | genericString)
              >> ')') | genericString)
              >> lit("->")
              >> (genericString)
              >> ';';

        bytes %= string("bytes")
              >> (('('
              >> (mathExpression | bracketedMathExpression | intRule | genericString)
              >> ')') | genericString)
              >> lit("->")
              >> (genericString)
              >> ';';

        bools %= string("bools")
              >> (('('
              >> (mathExpression | bracketedMathExpression | intRule | genericString)
              >> ')') | genericString)
              >> lit("->")
              >> (genericString)
              >> ';';

        strings %= string("strings")
              >> (('('
              >> (mathExpression | bracketedMathExpression | intRule | genericString)
              >> ')') | genericString)
              >> lit("->")
              >> (genericString)
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
        pairRule %= '(' >> (doubleRule | intRule | genericString) >> ','
                        >> (doubleRule | intRule | genericString) >> ')';

        // respresnts a triple of values e.g. (1, 2, 3)
        tupleRule %= '(' >> (doubleRule | intRule | genericString) >> ','
                         >> (doubleRule | intRule | genericString) >> ','
                         >> (doubleRule | intRule | genericString) >> ')';
    
        // prints out the value of a given variable
        query %= string("query") >> '('
              >> genericString
               >> ')' >> ';';

        // the entire set of instructions at out disposal
        commandCollection %= *allCommands;

        // a for-loop for iterating over elements in a list
        // e.g. for i in list {...}
        forLoop %= string("for")
                >> genericString // variable e.g. c
                >> lit("in")
                >> (genericString | bracketedWords) 
                >> '{'
                >>  commandCollection
                >> '}';

        // for a loop having the syntax
        // repeat N times {  }
        repeatLoop %= string("repeat") 
                    >> (intRule | genericString | bracketedMathExpression)
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
                    >> (doubleQuotedString | genericString)
                    >> ')'
                    >> ';';

        // the main entry point of a given program
        startFunction %= string("start")
                     >> '{'
                     >>  commandCollection
                     >> '}';

        // Rules defining parameter sets for functions
        parameter      %= (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                           mathExpression | bracketedMathExpression | 
                           comparisonExpression | bracketedComparisonExpression | bracketedWords);
        commaParameter %= ',' >> parameter;
        barString      %= '|' >> genericString;
        parameters     %= -(parameter >> *commaParameter); // comma-separated
        concats        %= (genericString >> *barString); // bar-separated
        // a collection of parameters
        // to be used by a function. This could be empty as in
        // () or have arguments, as in (a, b, c);
        parameterList %= parameters;

        // for string concatenation, use bar. As in (a|b|c)
        concatList    %= '(' >> concats >> ')';

        // a callable execution point
        block %= string("block")
              >> genericString // functionName
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
                        >> genericString // functionName
                        >> '(' >> parameterList >> ')' // list of parameters
                        >> -(lit("->") >> genericString)
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
                   >> genericString // return type
                   >> genericString // functionName
                   >> '(' >> parameterList >> ')' // list of parameters
                   >> -(lit("->") >> genericString)
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
             >> genericString // functionName
             >> '(' >> parameterList >> ')' // optional parameters
             // optional return part
             >> -(lit("->") >> genericString)
             >> ';';

        //  for setting a value in an array
        //  put 5 -> a(0);             
        put  %= string("put")
             >> (genericString | doubleRule | intRule | mathExpression | bracketedMathExpression)
             >> lit("->")
             >> genericString 
             // optional indexing
             >> -('('
             >> (mathExpression | bracketedMathExpression | intRule | genericString)
             >> ')')
             >> ';';

        // for getting a value in an array
        // get a(0) -> val;
        get  %= string("get")
             >> (genericString)
             >> '('
             >> (mathExpression | bracketedMathExpression | intRule | genericString)
             >> ')'
             >> lit("->")
             >> (genericString)
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
                        >> (genericString | doubleQuotedString)
                        >> lit(",")
                        >> (bracketedWords | genericString)
                        >> (')')
                        >> lit("->")
                        >> genericString 
                        >> ';';

        // get_token(0, [hello there]) -> t;
        listGetToken  %= string("get_token")
                      >> '('
                      >> (genericString | intRule)
                      >> ','
                      >> (bracketedWords | genericString)
                      >> ')'
                      >> lit("->")
                      >> genericString 
                      >> ';';

        // add_token("token", L);
        listAddToken  %= string("add_token")
                      >> '('
                      >> (genericString | doubleQuotedString | bracketedWords)
                      >> ','
                      >> (genericString)
                      >> ')'
                      >> ';';


        // set_token (0, [hello there], "goodbye") -> t;
        // set_token (0, [hello there], [nested bit]) -> t;
        listSetToken  %= string("set_token")
                      >> ('(')
                      >> (genericString | intRule)
                      >> (',')
                      >> (bracketedWords | genericString)
                      >> (',')
                      >> (genericString | doubleQuotedString | bracketedWords)
                      >> (')')
                      >> lit("->")
                      >> genericString 
                      >> ';';

        matchesCommand %= string("matches")
                       >> bracketedWords
                       >> '|'
                       >> bracketedWords
                       >> lit("->")
                       >> genericString 
                       >> ';';

        // appends to end of a string, s  
        // append (s, "hello") -> result;
        appendRule %= string("append")
                   >> ('(')
                   >> (doubleQuotedString | genericString)
                   >> ','
                   >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                       mathExpression | bracketedMathExpression | 
                       comparisonExpression | bracketedComparisonExpression)
                   >> ')'
                   >> lit("->")
                   >> genericString
                   >> ';';

        // string_reverse name; 
        reverseRule %= string("string_reverse")
                    >> (doubleQuotedString | genericString)
                    >> ';';

        // for printing out a string to screen
        pr %= string("pr")
             >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                 mathExpression | bracketedMathExpression | 
                 comparisonExpression | bracketedComparisonExpression)
             >> ';';

        // for printing out a string to screen with newline
        prn %= string("prn")
               >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                   mathExpression | bracketedMathExpression | 
                   comparisonExpression | bracketedComparisonExpression)
               >> ';';

        // equivalent to 'pr' -- slightly nicer syntax
        say %= string("say")
               >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                   mathExpression | bracketedMathExpression | 
                   comparisonExpression | bracketedComparisonExpression)
               >> ';';

        // lists all variables
        vars %= string("vars") >> brackets >> ';';

        // loads a jasl script
        loadScript %= string("script") 
                   >> doubleQuotedString >> ';';

        // erases a variable from the cache
        releaseCommand %= string("release")
                       >> genericString >> ';';

        // generates an int or real in somce range [0, n]
        // random:int n -> value;
        // random:real n -> value;
        randomCommand %= string("random") >> ':'
                      >> genericString
                      >> (genericString | doubleRule | intRule | 
                           mathExpression | bracketedMathExpression)
                      >> lit("->") 
                      >> genericString
                      >> ';';

        // for exiting a program
        exitCommand %= string("exit") >> -(genericString) >> ';';

        regexEqCommand %= string("regex_eq")
                       >> ('(') 
                       >> (doubleQuotedString | genericString) >> ','
                       >> (doubleQuotedString | genericString) 
                       >> (')')
                       >> lit("->")
                       >> genericString
                       >> ';';

        regexParseCommand %= string("regex_parse")
                          >> ('(') 
                          >> (doubleQuotedString | genericString) >> ','
                          >> (doubleQuotedString | genericString) 
                          >> (')')
                          >> lit("->")
                          >> genericString
                          >> ';';

        wildcardParseCommand %= string("wildcard_parse")
                             >> ('(') 
                             >> (doubleQuotedString | genericString) >> ','
                             >> (doubleQuotedString | genericString) 
                             >> (')')
                             >> lit("->")
                             >> genericString
                             >> ';';

        wildcardEqCommand %= string("wildcard_eq")
                          >> ('(') 
                          >> (doubleQuotedString | genericString) >> ','
                          >> (doubleQuotedString | genericString) 
                          >> (')')
                          >> lit("->")
                          >> genericString
                          >> ';';
        // concatenate strings. 
        concatRule %= string("concat")
                   >> (parameterList) >> lit("->")
                   >> genericString 
                   >> ';';

        // matches this type of command:
        // net_send something -> somethingElse;
        genericArrowRule %= genericString
                         >> (doubleQuotedString
                             | boolRule
                             | genericString
                             | doubleRule
                             | intRule
                             | mathExpression
                             | bracketedMathExpression
                             | comparisonExpression
                             | bracketedComparisonExpression
                             | bracketedWords)
                          >> lit("->")
                          >> (doubleQuotedString | genericString) 
                          >> ';';

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
                     | genericArrowRule;
                     
        start %= allCommands;
    }
}
