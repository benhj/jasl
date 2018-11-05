//
//  CommandParser.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "Function.hpp"
#include "CarrotString.hpp"
#include "LiteralString.hpp"
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
    using Iterator = std::string::const_iterator;

    struct CommandParser : qi::grammar<Iterator, Function(), ascii::space_type>
    {
        CommandParser();

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
        qi::rule<Iterator, Function(), ascii::space_type> say;
        qi::rule<Iterator, Function(), ascii::space_type> appendRule;
        qi::rule<Iterator, Function(), ascii::space_type> reverseRule;
        qi::rule<Iterator, Function(), ascii::space_type> listTokenIndex;
        qi::rule<Iterator, Function(), ascii::space_type> listGetToken;
        qi::rule<Iterator, Function(), ascii::space_type> listSetToken;
        qi::rule<Iterator, Function(), ascii::space_type> listAddToken;
        qi::rule<Iterator, Function(), ascii::space_type> execCommand;
        qi::rule<Iterator, Function(), ascii::space_type> releaseCommand;
        qi::rule<Iterator, Function(), ascii::space_type> randomCommand;
        qi::rule<Iterator, Function(), ascii::space_type> exitCommand;
        qi::rule<Iterator, ValueArray(), ascii::space_type> pairRule;
        qi::rule<Iterator, ValueArray(), ascii::space_type> tupleRule;
        qi::rule<Iterator, double(), ascii::space_type> doubleRule;
        qi::rule<Iterator, int64_t(), ascii::space_type> intRule;
        qi::rule<Iterator, bool(), ascii::space_type> boolRule;
        qi::rule<Iterator, MathExpression(), ascii::space_type> mathExpression;
        qi::rule<Iterator, MathExpression(), ascii::space_type> bracketedMathExpression;
        qi::rule<Iterator, ComparisonExpression(), ascii::space_type> comparisonExpression;
        qi::rule<Iterator, ComparisonExpression(), ascii::space_type> bracketedComparisonExpression;
        qi::rule<Iterator, std::string(), ascii::space_type> mathSymbols;
        qi::rule<Iterator, std::string(), ascii::space_type> comparisonSymbols;
        qi::rule<Iterator, std::vector<Function>(), ascii::space_type> commandCollection;

        // generic rule
        qi::rule<Iterator, Function(), ascii::space_type> concatRule;
        qi::rule<Iterator, Function(), ascii::space_type> genericArrowRule;

        // string manipulation new (2017-2018)
        qi::rule<Iterator, Function(), ascii::space_type> regexEqCommand;
        qi::rule<Iterator, Function(), ascii::space_type> wildcardEqCommand;
        qi::rule<Iterator, Function(), ascii::space_type> regexParseCommand;
        qi::rule<Iterator, Function(), ascii::space_type> wildcardParseCommand;

        // list manipulation (2018)
        qi::rule<Iterator, Function(), ascii::space_type> matchesCommand;

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
        qi::rule<Iterator, std::string(), ascii::space_type> commentPound;
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
