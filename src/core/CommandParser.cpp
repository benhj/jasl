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
#include "parser/Parameters.hpp"
#include "parser/ScreenIO.hpp"
#include "parser/Patterns.hpp"
#include "parser/ListManip.hpp"
#include "parser/StringManip.hpp"

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
        Parameters::init();
        ScreenIO::init();
        Patterns::init();
        ListManip::init();
        StringManip::init();

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

        // for running asynchronous (threaded) blocks of code
        asyncCommand %= string("async")
                     >> '{'
                     >>  commandCollection
                     >> '}';

        // a callable execution point
        block %= string("block")
              >> Strings::genericString // functionName
              // take an optional list of arguments
              >> '(' >> Parameters::parameterList >> ')'
              >> '{'
              >>  commandCollection
              >> '}';

        returnableArray %= string("fn")
                        >> ':'
                        >> ArrayTypes::arrayLexeme
                        >> ':' 
                        >> ArrayTypes::arrayTypes// return type
                        >> Strings::genericString // functionName
                        >> '(' >> Parameters::parameterList >> ')' // list of parameters
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
                   >> '(' >> Parameters::parameterList >> ')' // list of parameters
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
             >> '(' >> Parameters::parameterList >> ')' // optional parameters
             // optional return part
             >> -(lit("->") >> Strings::genericString)
             >> ';';

        // for setting a value
        // put 5 -> a;
        // put 10 -> b(1);       
        put  %= string("put")
             >> (Strings::genericString |
                 Strings::doubleQuotedString |
                 Strings::quotedString | 
                 BasicTypes::doubleRule |
                 BasicTypes::intRule |
                 Expressions::mathExpression |
                 Expressions::bracketedMathExpression)
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

        matchesCommand %= string("matches")
                       >> Strings::bracketedWords
                       >> '|'
                       >> Strings::bracketedWords
                       >> lit("->")
                       >> Strings::genericString 
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

        // For joining on all threads
        joinAllCommand %= string("join_all") >> -(Strings::genericString) >> ';';

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

        // all the instructions at out disposal
        allCommands %= forLoop
                     | query
                     | asyncCommand
                     | startFunction 
                     | block
                     | call 
                     | returnable
                     | returnableArray
                     | put
                     | ArrayTypes::array
                     | ScreenIO::echo
                     | ifRule 
                     | ifRule_B
                     | Comments::commentFunc
                     | vars 
                     | loadScript
                     | repeatLoop
                     | whileLoop
                     | StringManip::appendRule
                     | StringManip::reverseRule 
                     | ListManip::listGetToken
                     | ListManip::listSetToken
                     | ListManip::listAddToken
                     | ListManip::listTokenIndex
                     | ListManip::listMatches
                     | ListManip::listFind
                     | execCommand
                     | releaseCommand
                     | randomCommand
                     | exitCommand
                     | joinAllCommand
                     | get
                     | Patterns::pattern
                     | StringManip::concatRule
                     | ScreenIO::ansiUPRule
                     | genericArrowRule;
                     
        start %= allCommands;
    }
}
