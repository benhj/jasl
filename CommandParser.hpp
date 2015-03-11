//
//  CommandParser.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Function.hpp"
#include "LiteralString.hpp"
#include "SymbolString.hpp"
#include "commands/expressions/MathExpression.hpp"
#include "commands/expressions/ComparisonExpression.hpp"

#include <boost/any.hpp>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

namespace jasl
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    template <typename Iterator>
    struct CommandParser : qi::grammar<Iterator, Function(), ascii::space_type>
    {
        CommandParser() : CommandParser::base_type(start)
        {
            // token types that we're interested in using
            using qi::int_;
            using qi::lit;
            using qi::double_;
            using qi::bool_;
            using qi::lexeme;
            using qi::raw;
            using ascii::char_;
            using ascii::string;
            using ascii::blank;

            // convenience rules
            quotedString        %= lexeme['\'' >> +(char_ - '\'') >> '\''];
            doubleQuotedString  %= lexeme['\"' >> +(char_ - '\"') >> '\"'];
            genericString       %= lexeme[+(char_("a-zA-Z_"))];
            allChars            %= lexeme[+(char_ - '\n')];
            commentSlash        %= string("//");
            commentFunc         %= commentSlash >> -allChars;

            // a type that can be a double, an int or a string
            multiType %= (doubleRule | intRule | genericString);

            // A rule to represent the math syntax
            // m_add(5, 6, output) for binary math functions
            // OR
            // m_sin(5, output) for unary math functions
            mathRule %= string("m_")
                        >> genericString
                        >> '('
                        >> (doubleRule | intRule | genericString)
                        >> ','
                        >> (doubleRule | intRule | genericString)
                        >> ','
                        >> genericString
                        >> ')' >> ';'
                        |
                        string("m_")
                        >> genericString
                        >> '('
                        >> (doubleRule | intRule | genericString)
                        >> ','
                        >> genericString
                        >> ')' >> ';';

            // symbols to be used in a math expression
            mathSymbols %= lexeme[ string("%")
                                 | string("+")
                                 | string("-")
                                 | string("/")
                                 | string("*")];

            // logical symbols
            comparisonSymbols %= lexeme[ string("==")
                                       | string(">=")
                                       | string(">")
                                       | string("<=")
                                       | string("<")
                                       | string("&&")
                                       | string("||")
                                       | string("!=")];

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

            
            // Rules to permit c-style variable initialization
            // e.g. int i = 4;
            cIntRule %= string("int")
                     >> genericString
                     >> '='
                     >> (mathExpression | bracketedMathExpression | intRule | genericString)
                     >> ';';

            // e.g. double d = 5.0;
            cDoubleRule %= string("double")
                        >> genericString
                        >> '='
                        >> (mathExpression | bracketedMathExpression | doubleRule | genericString)
                        >> ';';

            // e.g. bool b = true;
            cBoolRule %= string("bool")
                      >> genericString
                      >> '='
                      >> (comparisonExpression | bracketedComparisonExpression | boolRule | genericString)
                      >> ';';

            // support c style initialization
            cVarRule %= (cBoolRule | cIntRule | cDoubleRule);


            //
            // with thanks to sehe and llonesmix @
            // http://stackoverflow.com/questions/15212671/
            // using-boostspirit-to-parse-multiple-types-of-single-value
            // #comment21459703_15212671
            //
            typedef boost::spirit::qi::strict_real_policies<double> DoublePolicy;
            typedef boost::spirit::qi::real_parser<double, DoublePolicy> DoubleParse;
            DoubleParse double_with_point;
            doubleRule %= (int_ >> 'f')
                        | (double_with_point >> -lit('f'));
            intRule %= int_;
            boolRule %= bool_;

            brackets %= "()";

            // represents a pair of values e.g. (5, 5)
            pairRule %= '(' >> (doubleRule | intRule | genericString) >> ','
                            >> (doubleRule | intRule | genericString) >> ')';

            // respresnts a triple of values e.g. (1, 2, 3)
            tupleRule %= '(' >> (doubleRule | intRule | genericString) >> ','
                             >> (doubleRule | intRule | genericString) >> ','
                             >> (doubleRule | intRule | genericString) >> ')';
        
            // for setting a variable 
            // (e.g. var(int, b, 5) will set an integer 'b' to the value 5)
            var %= string("var") 
                >> '('
                >> genericString >> ','
                >> genericString >> ','
                >> (doubleRule | intRule | boolRule | quotedString | genericString |
                    mathExpression | bracketedMathExpression | 
                    comparisonExpression | bracketedComparisonExpression)
                >> ')' >> ';';

            // prints out the value of a given variable
            query %= string("query") >> '('
                  >> genericString
                   >> ')' >> ';';

            // retrieves the value of a given type
            // e.g. get(int, a) will retrieve the value of the
            // a integer
            get  %= string("get") >> '('
                 >> genericString >> ','
                 >> genericString >> ')' >> ';';

            // the entire set of instructions at out disposal
            commandCollection %= *allCommands;

            // a simple for-loop
            // e.g. for(c = i : j) { ... }
            forLoop %= string("for")
                    >> '('
                    >> genericString // variable e.g. c
                    >> '='
                    >> (intRule | genericString | bracketedMathExpression) // variable e.g. i
                    >> ':'
                    >> (intRule | genericString | bracketedMathExpression) // variable e.g. j
                    >> ')'
                    >> '{'
                    >>  commandCollection
                    >> '}';

            // for a loop having the syntax
            // repeat N times {  }
            repeatLoop %= string("repeat") 
                        >> intRule
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

            // the main entry point of a given program
            startFunction %= string("start")
                         >> '{'
                         >>  commandCollection
                         >> '}';

            // a callable execution point
            block %= string("block")
                  >> genericString // functionName
                  >> '{'
                  >>  commandCollection
                  >> '}';

            // a simple if statement
            ifRule %= string("if")
                   >> '(' 
                   >>  comparisonExpression
                   >> ')'
                   >> '{'
                   >>  commandCollection
                   >> '}';

            // calls a subroutine with given name
            call %= string("call")
                 >> genericString // functionName
                 >> ';';

             // a string type
             // string(name, "hello");
            stringRule %= string("string")
                       >> '('
                       >> genericString >> ','
                       >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                           mathExpression | bracketedMathExpression | 
                           comparisonExpression | bracketedComparisonExpression)
                       >> ')'
                       >> ';';

            // for printing out a string to screen
            echo %= string("echo")
                 >> '(' 
                 >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                     mathExpression | bracketedMathExpression | 
                     comparisonExpression | bracketedComparisonExpression)
                 >> ')' >> ';';

            // for printing out a string to screen with newline
            echonl %= string("nlecho")
                   >> '(' 
                   >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                       mathExpression | bracketedMathExpression | 
                       comparisonExpression | bracketedComparisonExpression)
                   >> ')' >> ';';

            // lists all variables
            vars %= string("vars") >> brackets >> ';';

            // loads a jasl script
            loadScript %= string("script") >> '('
                       >> quotedString >> ')' >> ';';

            // all the instructions at out disposal
            allCommands %= var 
                         | get 
                         | forLoop
                         | query 
                         | startFunction 
                         | block
                         | call 
                         | mathRule 
                         | cVarRule 
                         | ifRule 
                         | commentFunc
                         | vars 
                         | loadScript
                         | echo
                         | echonl
                         | repeatLoop
                         | whileLoop
                         | stringRule;

            start %= allCommands;
        }

        // Individual rules will specify the grammar for the different
        // commandFunction types
        qi::rule<Iterator, Function(), ascii::space_type> get;
        qi::rule<Iterator, Function(), ascii::space_type> loadScript;
        qi::rule<Iterator, Function(), ascii::space_type> forLoop;
        qi::rule<Iterator, Function(), ascii::space_type> repeatLoop;
        qi::rule<Iterator, Function(), ascii::space_type> whileLoop;
        qi::rule<Iterator, Function(), ascii::space_type> commentFunc;
        qi::rule<Iterator, Function(), ascii::space_type> var;
        qi::rule<Iterator, Function(), ascii::space_type> vars;
        qi::rule<Iterator, Function(), ascii::space_type> query;
        qi::rule<Iterator, Function(), ascii::space_type> startFunction;
        qi::rule<Iterator, Function(), ascii::space_type> block;
        qi::rule<Iterator, Function(), ascii::space_type> call;
        qi::rule<Iterator, Function(), ascii::space_type> ifRule;
        qi::rule<Iterator, Function(), ascii::space_type> echo;
        qi::rule<Iterator, Function(), ascii::space_type> echonl;
        qi::rule<Iterator, Function(), ascii::space_type> stringRule;
        qi::rule<Iterator, ValueArray(), ascii::space_type> pairRule;
        qi::rule<Iterator, ValueArray(), ascii::space_type> tupleRule;
        qi::rule<Iterator, double(), ascii::space_type> doubleRule;
        qi::rule<Iterator, int(), ascii::space_type> intRule;
        qi::rule<Iterator, bool(), ascii::space_type> boolRule;
        qi::rule<Iterator, Value(), ascii::space_type> multiType;
        qi::rule<Iterator, Function(), ascii::space_type> mathRule;
        qi::rule<Iterator, Function(), ascii::space_type> cIntRule;
        qi::rule<Iterator, Function(), ascii::space_type> cDoubleRule;
        qi::rule<Iterator, Function(), ascii::space_type> cBoolRule;
        qi::rule<Iterator, Function(), ascii::space_type> cVarRule;
        qi::rule<Iterator, MathExpression(), ascii::space_type> mathExpression;
        qi::rule<Iterator, MathExpression(), ascii::space_type> bracketedMathExpression;
        qi::rule<Iterator, ComparisonExpression(), ascii::space_type> comparisonExpression;
        qi::rule<Iterator, ComparisonExpression(), ascii::space_type> bracketedComparisonExpression;
        qi::rule<Iterator, std::string(), ascii::space_type> mathSymbols;
        qi::rule<Iterator, std::string(), ascii::space_type> comparisonSymbols;
        qi::rule<Iterator, std::vector<Function>(), ascii::space_type> commandCollection;

        // Core rule declarations
        qi::rule<Iterator, std::string(), ascii::space_type> allChars;
        qi::rule<Iterator, std::string(), ascii::space_type> commentSlash;
        qi::rule<Iterator, std::string(), ascii::space_type> genericString;
        qi::rule<Iterator, LiteralString(), ascii::space_type> quotedString;
        qi::rule<Iterator, LiteralString(), ascii::space_type> doubleQuotedString;
        qi::rule<Iterator, std::string(), ascii::space_type> brackets;
        qi::rule<Iterator, Function(), ascii::space_type> start;
        qi::rule<Iterator, Function(), ascii::space_type> allCommands;
    };
}
