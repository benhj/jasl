//
//  CommandParser.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2017 Ben Jones. All rights reserved.
//

#pragma once

#include "Function.hpp"
#include "CarrotString.hpp"
#include "LiteralString.hpp"
#include "SymbolString.hpp"
#include "commands/expressions/MathExpression.hpp"
#include "commands/expressions/ComparisonExpression.hpp"

#include <boost/variant.hpp>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <unicode/unistr.h>
#include <unicode/bytestream.h>

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
            commentFunc         %= (commentColons | commentSlash) >> -allChars;
            word                %= lexeme[+(char_ - ' ' - ',' - '[' - ']' - '(' - ')')];
            carrotWord          %= lexeme['^' >> +(char_ - '^')];

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

            // int 4 -> i;
            intNewSyntax %= string("int")
                         >> (mathExpression | bracketedMathExpression | intRule | genericString | doubleQuotedString)
                         >> lit("->")
                         >> (genericString)
                         >> ';';

            byteNewSyntax %= string("byte")
                          >> (mathExpression | bracketedMathExpression | intRule | genericString)
                          >> lit("->")
                          >> (genericString)
                          >> ';';

            // real 5.0 -> d; 
            doubleNewSyntax %= string("real")
                            >> (mathExpression | bracketedMathExpression | doubleRule | genericString | doubleQuotedString)
                            >> lit("->")
                            >> (genericString)
                            >> ';';

            // bool true -> b;
            boolNewSyntax %= string("bool")
                          >> (comparisonExpression | bracketedComparisonExpression | boolRule | genericString | doubleQuotedString)
                          >> lit("->")
                          >> (genericString)
                          >> ';';

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

            // string or array length
            // length "hello" -> len;
            stringLengthRule %= string("length")
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
                           comparisonExpression | bracketedComparisonExpression | bracketedWords
                           | concatList) >> lit("->")
                       >> genericString 
                       >> ';';
       
            // concatenate strings
            concatRule %= string("concat")
                       >> (parameterList) >> lit("->")
                       >> genericString 
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
            stringList     %= string("list")
                           >> (bracketedWords | genericString)
                           >> lit("->")
                           >> genericString // the name of the list
                           >> ';';

            // list_to_string [hello there] -> s;
            listToString  %= string("list_to_string")
                          >> (bracketedWords | genericString)
                          >> lit("->")
                          >> genericString 
                          >> ';';

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

            // appends to end of a string, s  
            // append (s, "hello") --> result;
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

            // will try and convert a string to an int
            stringToIntRule %= string("string_to_int")
                            >> (doubleQuotedString | genericString) 
                            >> lit("->")
                            >> genericString
                            >> ';';

            // will try and convert a string to a double
            stringToRealRule %= string("string_to_real")
                               >> (doubleQuotedString | genericString) 
                               >> lit("->")
                               >> genericString
                               >> ';';

            argsRule %= string("args")
                     >> (intRule | genericString) 
                     >> lit("->")
                     >> genericString
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

            // lists all variables
            vars %= string("vars") >> brackets >> ';';

            // loads a jasl script
            loadScript %= string("script") 
                       >> doubleQuotedString >> ';';

            // erases a variable from the cache
            releaseCommand %= string("release")
                           >> genericString >> ';';

            // determines the type of a variable and stores in a string
            typeCommand %= string("type") 
                        >> genericString
                        >> lit("->") 
                        >> genericString
                        >> ';';

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

            // list folder
            // folder_list "/path/to/folder" -> strings;
            folderList %= string("folder_list")
                       >> (doubleQuotedString | genericString) >> lit("->")
                       >> genericString 
                       >> ';';

            // read lines from file       
            fileReadLines %= string("file_read_lines")
                          >> (doubleQuotedString | genericString) >> lit("->")
                          >> genericString 
                          >> ';';

            // read bytes from file       
            fileReadBytes %= string("file_read_bytes")
                          >> (doubleQuotedString | genericString) >> lit("->")
                          >> genericString 
                          >> ';';

            // write bytes to file       
            fileWriteBytes %= string("file_write_bytes")
                           >> (genericString) >> lit("->")
                           >> (doubleQuotedString | genericString) 
                           >> ';';

            // append bytes to file       
            fileAppendBytes %= string("file_append_bytes")
                            >> (genericString) >> lit("->")
                            >> (doubleQuotedString | genericString) 
                            >> ';';


            fileAppendLine %= string("file_append_line")
                            >> (genericString) >> lit("->")
                            >> (doubleQuotedString | genericString) 
                            >> ';';

            matchesCommand %= string("matches")
                         >> ('(') 
                         >> (doubleQuotedString | genericString) >> ','
                         >> (doubleQuotedString | genericString) 
                         >> (')')
                         >> lit("->")
                         >> genericString
                         >> ';';

            // opens a tcp connection. When implemented, will allow
            // tcp_connect server:port -> descriptor;
            tcpConnect %= string("tcp_connect")
                       >> (doubleQuotedString | genericString) >> ':'
                       >> (genericString | intRule) >> lit("->")
                       >> genericString
                       >> ';';

            // Reads from a tcp connection. When implemented, will allow
            // net_read fd -> bytes;
            netRead %= string("net_read")
                    >> (genericString | intRule) >> lit("->")
                    >> genericString
                    >> ';';

            //Writes to a tcp connection. When implemented, will allow
            // net_write bytes -> fd;
            netWrite %= string("net_write")
                     >> (doubleQuotedString | genericString) >> lit("->")
                     >> (genericString | intRule)
                     >> ';';

            // all the instructions at out disposal
            allCommands %= forLoop
                         | query 
                         | startFunction 
                         | block
                         | call 
                         | returnable
                         | returnableArray
                         | ints | bytes | strings | bools | reals
                         | intNewSyntax
                         | byteNewSyntax
                         | doubleNewSyntax
                         | boolNewSyntax
                         | ifRule 
                         | ifRule_B
                         | argsRule
                         | commentFunc
                         | vars 
                         | loadScript
                         | prn
                         | pr
                         | repeatLoop
                         | whileLoop
                         | appendRule
                         | reverseRule 
                         | stringToRealRule
                         | stringToIntRule
                         | stringLengthRule
                         | stringRule
                         | concatRule
                         | listGetToken
                         | listSetToken
                         | listAddToken
                         | listToString
                         | listTokenIndex
                         | stringList
                         | inputRule
                         | execCommand
                         | releaseCommand
                         | typeCommand
                         | randomCommand
                         | exitCommand
                         | put
                         | get
                         | folderList
                         | fileReadLines
                         | fileReadBytes
                         | fileWriteBytes
                         | fileAppendBytes
                         | fileAppendLine
                         | matchesCommand
                         | tcpConnect
                         | netRead
                         | netWrite;
                         
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
        qi::rule<Iterator, Function(), ascii::space_type> ints;
        qi::rule<Iterator, Function(), ascii::space_type> bytes;
        qi::rule<Iterator, Function(), ascii::space_type> strings;
        qi::rule<Iterator, Function(), ascii::space_type> bools;
        qi::rule<Iterator, Function(), ascii::space_type> reals;
        qi::rule<Iterator, Function(), ascii::space_type> put;
        qi::rule<Iterator, Function(), ascii::space_type> get;
        qi::rule<Iterator, Function(), ascii::space_type> startFunction;
        qi::rule<Iterator, Function(), ascii::space_type> block;
        qi::rule<Iterator, Function(), ascii::space_type> call;
        qi::rule<Iterator, Function(), ascii::space_type> returnable;
        qi::rule<Iterator, Function(), ascii::space_type> returnableArray;
        qi::rule<Iterator, Function(), ascii::space_type> ifRule;
        qi::rule<Iterator, Function(), ascii::space_type> ifRule_B;
        qi::rule<Iterator, Function(), ascii::space_type> pr;
        qi::rule<Iterator, Function(), ascii::space_type> prn;
        qi::rule<Iterator, Function(), ascii::space_type> stringRule;
        qi::rule<Iterator, Function(), ascii::space_type> concatRule;
        qi::rule<Iterator, Function(), ascii::space_type> stringLengthRule;
        qi::rule<Iterator, Function(), ascii::space_type> appendRule;
        qi::rule<Iterator, Function(), ascii::space_type> reverseRule;
        qi::rule<Iterator, Function(), ascii::space_type> stringList;
        qi::rule<Iterator, Function(), ascii::space_type> argsRule;
        qi::rule<Iterator, Function(), ascii::space_type> stringToIntRule;
        qi::rule<Iterator, Function(), ascii::space_type> stringToRealRule;
        qi::rule<Iterator, Function(), ascii::space_type> inputRule;
        qi::rule<Iterator, Function(), ascii::space_type> listToString;
        qi::rule<Iterator, Function(), ascii::space_type> listTokenIndex;
        qi::rule<Iterator, Function(), ascii::space_type> listGetToken;
        qi::rule<Iterator, Function(), ascii::space_type> listSetToken;
        qi::rule<Iterator, Function(), ascii::space_type> listAddToken;
        qi::rule<Iterator, Function(), ascii::space_type> execCommand;
        qi::rule<Iterator, Function(), ascii::space_type> releaseCommand;
        qi::rule<Iterator, Function(), ascii::space_type> typeCommand;
        qi::rule<Iterator, Function(), ascii::space_type> randomCommand;
        qi::rule<Iterator, Function(), ascii::space_type> exitCommand;
        qi::rule<Iterator, ValueArray(), ascii::space_type> pairRule;
        qi::rule<Iterator, ValueArray(), ascii::space_type> tupleRule;
        qi::rule<Iterator, double(), ascii::space_type> doubleRule;
        qi::rule<Iterator, int64_t(), ascii::space_type> intRule;
        qi::rule<Iterator, bool(), ascii::space_type> boolRule;
        qi::rule<Iterator, Function(), ascii::space_type> intNewSyntax;
        qi::rule<Iterator, Function(), ascii::space_type> byteNewSyntax;
        qi::rule<Iterator, Function(), ascii::space_type> doubleNewSyntax;
        qi::rule<Iterator, Function(), ascii::space_type> boolNewSyntax;
        qi::rule<Iterator, MathExpression(), ascii::space_type> mathExpression;
        qi::rule<Iterator, MathExpression(), ascii::space_type> bracketedMathExpression;
        qi::rule<Iterator, ComparisonExpression(), ascii::space_type> comparisonExpression;
        qi::rule<Iterator, ComparisonExpression(), ascii::space_type> bracketedComparisonExpression;
        qi::rule<Iterator, std::string(), ascii::space_type> mathSymbols;
        qi::rule<Iterator, std::string(), ascii::space_type> comparisonSymbols;
        qi::rule<Iterator, std::vector<Function>(), ascii::space_type> commandCollection;

        // file i/o
        qi::rule<Iterator, Function(), ascii::space_type> folderList;
        qi::rule<Iterator, Function(), ascii::space_type> fileReadLines;
        qi::rule<Iterator, Function(), ascii::space_type> fileReadBytes;
        qi::rule<Iterator, Function(), ascii::space_type> fileWriteBytes;
        qi::rule<Iterator, Function(), ascii::space_type> fileAppendBytes;
        qi::rule<Iterator, Function(), ascii::space_type> fileAppendLine;

        // string manipulation new (2017)
        qi::rule<Iterator, Function(), ascii::space_type> matchesCommand;

        // net i/o
        qi::rule<Iterator, Function(), ascii::space_type> tcpConnect;
        qi::rule<Iterator, Function(), ascii::space_type> netRead;
        qi::rule<Iterator, Function(), ascii::space_type> netWrite;

        // Core rule declarations
        qi::rule<Iterator, std::string(), ascii::space_type> word;
        qi::rule<Iterator, Value(), ascii::space_type> parameter;
        qi::rule<Iterator, Value(), ascii::space_type> bracketedWords;
        qi::rule<Iterator, Value(), ascii::space_type> commaParameter;
        qi::rule<Iterator, Value(), ascii::space_type> parameterList;
        qi::rule<Iterator, ValueArray(), ascii::space_type> words;
        qi::rule<Iterator, ValueArray(), ascii::space_type> parameters;
        qi::rule<Iterator, std::string(), ascii::space_type> barString;
        qi::rule<Iterator, StringArray(), ascii::space_type> concats;
        qi::rule<Iterator, Value(), ascii::space_type> concatList;
        qi::rule<Iterator, CarrotString(), ascii::space_type> carrotWord;
        qi::rule<Iterator, std::string(), ascii::space_type> arrayLexeme;
        qi::rule<Iterator, std::string(), ascii::space_type> arrayTypes;
        qi::rule<Iterator, std::string(), ascii::space_type> allChars;
        qi::rule<Iterator, std::string(), ascii::space_type> commentSlash;
        qi::rule<Iterator, std::string(), ascii::space_type> commentColons;
        qi::rule<Iterator, std::string(), ascii::space_type> genericString;
        qi::rule<Iterator, char()> escChar;

        struct cp2utf8_f
        {
            template < typename ... > struct result { using type = void; };

            void operator()( std::string & a, UChar32 codepoint ) const
            {
                icu::StringByteSink< std::string > bs( &a );
                icu::UnicodeString::fromUTF32( &codepoint, 1 ).toUTF8( bs );
            }
        };

        ::boost::phoenix::function< cp2utf8_f > cp2utf8;

        qi::rule< Iterator, std::string() > escapes;
        qi::symbols< char const, UChar32 > escaped_character;
        qi::rule<Iterator, LiteralString(), ascii::space_type> quotedString;
        qi::rule<Iterator, LiteralString(), ascii::space_type> doubleQuotedString;
        qi::rule<Iterator, std::string(), ascii::space_type> brackets;
        qi::rule<Iterator, Function(), ascii::space_type> start;
        qi::rule<Iterator, Function(), ascii::space_type> allCommands;
    };
}
