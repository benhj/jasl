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
        qi::rule<Iterator, Function(), ascii::space_type> vars;
        qi::rule<Iterator, Function(), ascii::space_type> query;
        qi::rule<Iterator, Function(), ascii::space_type> put;
        qi::rule<Iterator, Function(), ascii::space_type> get;
        qi::rule<Iterator, Function(), ascii::space_type> startFunction;
        qi::rule<Iterator, Function(), ascii::space_type> block;
        qi::rule<Iterator, Function(), ascii::space_type> call;
        qi::rule<Iterator, Function(), ascii::space_type> returnable;
        qi::rule<Iterator, Function(), ascii::space_type> returnableArray;
        qi::rule<Iterator, Function(), ascii::space_type> ifRule;
        qi::rule<Iterator, Function(), ascii::space_type> ifRule_B;

        qi::rule<Iterator, Function(), ascii::space_type> execCommand;
        qi::rule<Iterator, Function(), ascii::space_type> releaseCommand;
        qi::rule<Iterator, Function(), ascii::space_type> randomCommand;
        qi::rule<Iterator, Function(), ascii::space_type> exitCommand;
        qi::rule<Iterator, std::vector<Function>(), ascii::space_type> commandCollection;

        // generic rule
        qi::rule<Iterator, Function(), ascii::space_type> genericArrowRule;

        // list manipulation (2018)
        qi::rule<Iterator, Function(), ascii::space_type> matchesCommand;

        // Core rule declarations
        qi::rule<Iterator, std::string(), ascii::space_type> arrayLexeme;

        qi::rule<Iterator, std::string(), ascii::space_type> brackets;
        qi::rule<Iterator, Function(), ascii::space_type> start;
        qi::rule<Iterator, Function(), ascii::space_type> allCommands;
    };
}
