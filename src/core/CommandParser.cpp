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
#include "parser/Symbols.hpp"
#include "parser/Comments.hpp"
#include "parser/BasicTypes.hpp"
#include "parser/Expressions.hpp"
#include "parser/ArrayTypes.hpp"

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
        Symbols::init();
        Comments::init();
        BasicTypes::init();
        Expressions::init();
        ArrayTypes::init();

        brackets %= "()";
    
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
                >> (Strings::genericString | Strings::bracketedWords) 
                >> '{'
                >>  commandCollection
                >> '}';

        // for a loop having the syntax
        // repeat N times {  }
        repeatLoop %= string("repeat") 
                    >> (BasicTypes::intRule | Strings::genericString | Expressions::bracketedMathExpression)
                    >> lit("times")
                    >> '{'
                    >> commandCollection
                    >> '}';

        // for a loop having the syntax
        // while (condition) {}
        whileLoop %= string("while") 
                  >> '(' 
                  >> (BasicTypes::boolRule | Expressions::comparisonExpression | Expressions::bracketedComparisonExpression)
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
        parameter      %= (Strings::doubleQuotedString | Strings::genericString | BasicTypes::doubleRule | BasicTypes::intRule | BasicTypes::boolRule | 
                           Expressions::mathExpression | Expressions::bracketedMathExpression | 
                           Expressions::comparisonExpression | Expressions::bracketedComparisonExpression | Strings::bracketedWords);
        commaParameter %= ',' >> parameter;
        parameters     %= -(parameter >> *commaParameter); // comma-separated
        // a collection of parameters
        // to be used by a function. This could be empty as in
        // () or have arguments, as in (a, b, c);
        parameterList %= parameters;

        // a callable execution point
        block %= string("block")
              >> Strings::genericString // functionName
              // take an optional list of arguments
              >> '(' >> parameterList >> ')'
              >> '{'
              >>  commandCollection
              >> '}';

        returnableArray %= string("fn")
                        >> ':'
                        >> ArrayTypes::arrayLexeme
                        >> ':' 
                        >> ArrayTypes::arrayTypes// return type
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
               >>  Expressions::comparisonExpression
               >> ')'
               >> '{'
               >>  commandCollection
               >> '}'
               // optional 'else part'
               >> -(lit("else") >> '{' >> commandCollection >> '}');

        ifRule_B %= string("?")
               >> '(' 
               >>  Expressions::comparisonExpression
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
             >> (Strings::genericString | BasicTypes::doubleRule | BasicTypes::intRule | Expressions::mathExpression | Expressions::bracketedMathExpression)
             >> lit("->")
             >> Strings::genericString 
             // optional indexing
             >> -('('
             >> (Expressions::mathExpression | Expressions::bracketedMathExpression | BasicTypes::intRule | Strings::genericString)
             >> ')')
             >> ';';

        // for getting a value in an array
        // get a(0) -> val;
        get  %= string("get")
             >> (Strings::genericString)
             >> '('
             >> (Expressions::mathExpression | Expressions::bracketedMathExpression | BasicTypes::intRule | Strings::genericString)
             >> ')'
             >> lit("->")
             >> (Strings::genericString)
             >> ';';

        // index_of ("hello", [hello there]) -> s;
        listTokenIndex  %= string("index_of")
                        >> '('
                        >> (Strings::genericString | Strings::doubleQuotedString)
                        >> lit(",")
                        >> (Strings::bracketedWords | Strings::genericString)
                        >> (')')
                        >> lit("->")
                        >> Strings::genericString 
                        >> ';';

        // get_token(0, [hello there]) -> t;
        listGetToken  %= string("get_token")
                      >> '('
                      >> (Strings::genericString | BasicTypes::intRule)
                      >> ','
                      >> (Strings::bracketedWords | Strings::genericString)
                      >> ')'
                      >> lit("->")
                      >> Strings::genericString 
                      >> ';';

        // add_token("token", L);
        listAddToken  %= string("add_token")
                      >> '('
                      >> (Strings::genericString | Strings::doubleQuotedString | Strings::bracketedWords)
                      >> ','
                      >> (Strings::genericString)
                      >> ')'
                      >> ';';

        // set_token (0, [hello there], "goodbye") -> t;
        // set_token (0, [hello there], [nested bit]) -> t;
        listSetToken  %= string("set_token")
                      >> ('(')
                      >> (Strings::genericString | BasicTypes::intRule)
                      >> (',')
                      >> (Strings::bracketedWords | Strings::genericString)
                      >> (',')
                      >> (Strings::genericString | Strings::doubleQuotedString | Strings::bracketedWords)
                      >> (')')
                      >> lit("->")
                      >> Strings::genericString 
                      >> ';';

        matchesCommand %= string("matches")
                       >> Strings::bracketedWords
                       >> '|'
                       >> Strings::bracketedWords
                       >> lit("->")
                       >> Strings::genericString 
                       >> ';';

        // appends to end of a string, s  
        // append (s, "hello") -> result;
        appendRule %= string("append")
                   >> ('(')
                   >> (Strings::doubleQuotedString | Strings::genericString)
                   >> ','
                   >> (Strings::doubleQuotedString | Strings::genericString | BasicTypes::doubleRule | BasicTypes::intRule | BasicTypes::boolRule | 
                       Expressions::mathExpression | Expressions::bracketedMathExpression | 
                       Expressions::comparisonExpression | Expressions::bracketedComparisonExpression)
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
             >> (Strings::doubleQuotedString | Strings::genericString | BasicTypes::doubleRule | BasicTypes::intRule | BasicTypes::boolRule | 
                 Expressions::mathExpression | Expressions::bracketedMathExpression | 
                 Expressions::comparisonExpression | Expressions::bracketedComparisonExpression)
             >> ';';

        // for printing out a string to screen with newline
        prn %= string("prn")
               >> (Strings::doubleQuotedString | Strings::genericString | BasicTypes::doubleRule | BasicTypes::intRule | BasicTypes::boolRule | 
                   Expressions::mathExpression | Expressions::bracketedMathExpression | 
                   Expressions::comparisonExpression | Expressions::bracketedComparisonExpression)
               >> ';';

        // equivalent to 'pr' -- slightly nicer syntax
        say %= string("say")
               >> (Strings::doubleQuotedString | Strings::genericString | BasicTypes::doubleRule | BasicTypes::intRule | BasicTypes::boolRule | 
                   Expressions::mathExpression | Expressions::bracketedMathExpression | 
                   Expressions::comparisonExpression | Expressions::bracketedComparisonExpression)
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
                      >> (Strings::genericString | BasicTypes::doubleRule | BasicTypes::intRule | 
                           Expressions::mathExpression | Expressions::bracketedMathExpression)
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
                             | BasicTypes::boolRule
                             | Strings::genericString
                             | BasicTypes::doubleRule
                             | BasicTypes::intRule
                             | Expressions::mathExpression
                             | Expressions::bracketedMathExpression
                             | Expressions::comparisonExpression
                             | Expressions::bracketedComparisonExpression
                             | Strings::bracketedWords)
                          >> lit("->")
                          >> (Strings::doubleQuotedString | Strings::genericString) 
                          >> ';';

        // matches this type of command:
        // net_send something;
        ansiUPRule %= string("ansi_up")
                   >> (Strings::genericString
                      | BasicTypes::intRule
                      | Expressions::mathExpression
                      | Expressions::bracketedMathExpression) >> ';';

        // all the instructions at out disposal
        allCommands %= forLoop
                     | query 
                     | startFunction 
                     | block
                     | call 
                     | returnable
                     | returnableArray
                     | put
                     | ArrayTypes::ints
                     | ArrayTypes::bytes
                     | ArrayTypes::strings
                     | ArrayTypes::bools
                     | ArrayTypes::reals
                     | prn
                     | pr
                     | say
                     | ifRule 
                     | ifRule_B
                     | Comments::commentFunc
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
