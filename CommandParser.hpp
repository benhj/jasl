//
//  CommandParser.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Function.hpp"
#include "CarrotString.hpp"
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

#include <cstdint>

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
            using qi::long_long;
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
            word                %= lexeme[+(char_ - ' ' - '[' - ']')];
            carrotWord          %= lexeme['^' >> +(char_ - '^')];

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

            // integer 4 -> i;
            intNewSyntax %= string("integer")
                         >> (mathExpression | bracketedMathExpression | intRule | genericString | doubleQuotedString)
                         >> lit("->")
                         >> (genericString)
                         >> ';';

            // decimal 5.0 -> d; 
            doubleNewSyntax %= string("decimal")
                            >> (mathExpression | bracketedMathExpression | doubleRule | genericString | doubleQuotedString)
                            >> lit("->")
                            >> (genericString)
                            >> ';';

            // boolean true -> b;
            boolNewSyntax %= string("boolean")
                          >> (comparisonExpression | bracketedComparisonExpression | boolRule | genericString | doubleQuotedString)
                          >> lit("->")
                          >> (genericString)
                          >> ';';

            //
            // with thanks to sehe and llonesmix @
            // http://stackoverflow.com/questions/15212671/
            // using-boostspirit-to-parse-multiple-types-of-single-value
            // #comment21459703_15212671
            //
            typedef boost::spirit::qi::strict_real_policies<double> DoublePolicy;
            typedef boost::spirit::qi::real_parser<double, DoublePolicy> DoubleParse;
            DoubleParse double_with_point;
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

            // string length
            // string_length "hello" -> len;
            stringLengthRule %= string("string_length")
                             >> (doubleQuotedString | genericString) 
                             >> lit("->")
                             >> genericString
                             >> ';';

            // queries for user input
            // input "What do you want ?" -> s;
            inputRule %= string("input")
                      >> (doubleQuotedString | genericString) >> lit("->")
                      >> genericString 
                      >> ';';

            // string type
            // string "hello" -> name;
            stringRule %= string("string")
                       >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                           mathExpression | bracketedMathExpression | 
                           comparisonExpression | bracketedComparisonExpression | bracketedWords) >> lit("->")
                       >> genericString 
                       >> ';';




            // a list of words
            // e.g. list [hello there] -> L;
            // will be useful for pattern matching
            // a collection of words
            words          %= (*word); // zero or more words
            bracketedWords %= '[' >> words >> ']';
            stringList     %= string("list")
                           >> bracketedWords
                           >> lit("->")
                           >> genericString // the name of the list
                           >> ';';

            // list_to_string [hello there] -> s;
            listToString  %= string("list_to_string")
                          >> (bracketedWords | genericString)
                          >> lit("->")
                          >> genericString 
                          >> ';';

            // list_token_index "hello" in [hello there] -> s;
            listTokenIndex  %= string("index of")
                            >> (genericString | doubleQuotedString)
                            >> lit("in")
                            >> (bracketedWords | genericString)
                            >> lit("->")
                            >> genericString 
                            >> ';';

            // appends to a string
            // append "hello" to end of s -> result;    
            appendRule %= string("append")
                       >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                           mathExpression | bracketedMathExpression | 
                           comparisonExpression | bracketedComparisonExpression)
                       >> lit("to end of")
                       >> (doubleQuotedString | genericString)
                       >> lit("->")
                       >> genericString
                       >> ';';

            // string_reverse name; 
            reverseRule %= string("string_reverse")
                        >> (doubleQuotedString | genericString)
                        >> ';';

            // will try and convert a string to an int
            stringToIntRule %= string("string_to_integer")
                            >> (doubleQuotedString | genericString) 
                            >> lit("->")
                            >> genericString
                            >> ';';

            // will try and convert a string to a double
            stringToDoubleRule %= string("string_to_decimal")
                               >> (doubleQuotedString | genericString) 
                               >> lit("->")
                               >> genericString
                               >> ';';

            argsRule %= string("args")
                     >> intRule >> lit("->")
                     >> genericString
                     >> ';';

            // for printing out a string to screen
            echo %= string("echo")
                 >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                     mathExpression | bracketedMathExpression | 
                     comparisonExpression | bracketedComparisonExpression)
                 >> ';';

            // for printing out a string to screen with newline
            echo_nl %= string("echo_nl")
                   >> (doubleQuotedString | genericString | doubleRule | intRule | boolRule | 
                       mathExpression | bracketedMathExpression | 
                       comparisonExpression | bracketedComparisonExpression)
                   >> ';';

            // lists all variables
            vars %= string("vars") >> brackets >> ';';

            // loads a jasl script
            loadScript %= string("script") >> '('
                       >> quotedString >> ')' >> ';';

            // all the instructions at out disposal
            allCommands %= forLoop
                         | query 
                         | startFunction 
                         | block
                         | call 
                         | intNewSyntax
                         | doubleNewSyntax
                         | boolNewSyntax
                         | ifRule 
                         | argsRule
                         | commentFunc
                         | vars 
                         | loadScript
                         | echo_nl
                         | echo
                         | repeatLoop
                         | whileLoop
                         | appendRule
                         | reverseRule 
                         | stringToDoubleRule
                         | stringToIntRule
                         | stringLengthRule
                         | stringRule
                         | listToString
                         | listTokenIndex
                         | stringList
                         | inputRule;
                         
            start %= allCommands;
        }

        // Individual rules will specify the grammar for the different
        // commandFunction types
        qi::rule<Iterator, Function(), ascii::space_type> loadScript;
        qi::rule<Iterator, Function(), ascii::space_type> forLoop;
        qi::rule<Iterator, Function(), ascii::space_type> repeatLoop;
        qi::rule<Iterator, Function(), ascii::space_type> whileLoop;
        qi::rule<Iterator, Function(), ascii::space_type> commentFunc;
        qi::rule<Iterator, Function(), ascii::space_type> vars;
        qi::rule<Iterator, Function(), ascii::space_type> query;
        qi::rule<Iterator, Function(), ascii::space_type> startFunction;
        qi::rule<Iterator, Function(), ascii::space_type> block;
        qi::rule<Iterator, Function(), ascii::space_type> call;
        qi::rule<Iterator, Function(), ascii::space_type> ifRule;
        qi::rule<Iterator, Function(), ascii::space_type> echo;
        qi::rule<Iterator, Function(), ascii::space_type> echo_nl;
        qi::rule<Iterator, Function(), ascii::space_type> stringRule;
        qi::rule<Iterator, Function(), ascii::space_type> stringLengthRule;
        qi::rule<Iterator, Function(), ascii::space_type> appendRule;
        qi::rule<Iterator, Function(), ascii::space_type> reverseRule;
        qi::rule<Iterator, Function(), ascii::space_type> stringList;
        qi::rule<Iterator, Function(), ascii::space_type> argsRule;
        qi::rule<Iterator, Function(), ascii::space_type> stringToIntRule;
        qi::rule<Iterator, Function(), ascii::space_type> stringToDoubleRule;
        qi::rule<Iterator, Function(), ascii::space_type> inputRule;
        qi::rule<Iterator, Function(), ascii::space_type> listToString;
        qi::rule<Iterator, Function(), ascii::space_type> listTokenIndex;
        qi::rule<Iterator, ValueArray(), ascii::space_type> pairRule;
        qi::rule<Iterator, ValueArray(), ascii::space_type> tupleRule;
        qi::rule<Iterator, double(), ascii::space_type> doubleRule;
        qi::rule<Iterator, int64_t(), ascii::space_type> intRule;
        qi::rule<Iterator, bool(), ascii::space_type> boolRule;
        qi::rule<Iterator, Function(), ascii::space_type> intNewSyntax;
        qi::rule<Iterator, Function(), ascii::space_type> doubleNewSyntax;
        qi::rule<Iterator, Function(), ascii::space_type> boolNewSyntax;
        qi::rule<Iterator, MathExpression(), ascii::space_type> mathExpression;
        qi::rule<Iterator, MathExpression(), ascii::space_type> bracketedMathExpression;
        qi::rule<Iterator, ComparisonExpression(), ascii::space_type> comparisonExpression;
        qi::rule<Iterator, ComparisonExpression(), ascii::space_type> bracketedComparisonExpression;
        qi::rule<Iterator, std::string(), ascii::space_type> mathSymbols;
        qi::rule<Iterator, std::string(), ascii::space_type> comparisonSymbols;
        qi::rule<Iterator, std::vector<Function>(), ascii::space_type> commandCollection;

        // Core rule declarations
        // qi::rule<Iterator, Function(), ascii::space_type> bracketedWord;
        qi::rule<Iterator, std::string(), ascii::space_type> word;
        qi::rule<Iterator, Value(), ascii::space_type> bracketedWords;
        qi::rule<Iterator, ValueArray(), ascii::space_type> words;
        qi::rule<Iterator, CarrotString(), ascii::space_type> carrotWord;
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
