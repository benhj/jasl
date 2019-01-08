// Ben Jones 2019

#pragma once

#include "commands/expressions/MathExpression.hpp"
#include "commands/expressions/ComparisonExpression.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>

namespace jasl {
    class Expressions {
      public:
        using Iterator = std::string::const_iterator;

        static ::boost::spirit::qi::rule< Iterator,
                                          MathExpression(),
                                          ::boost::spirit::ascii::space_type> mathExpression;
        static ::boost::spirit::qi::rule< Iterator,
                                          MathExpression(),
                                          ::boost::spirit::ascii::space_type> bracketedMathExpression;
        static ::boost::spirit::qi::rule< Iterator,
                                          ComparisonExpression(),
                                          ::boost::spirit::ascii::space_type> comparisonExpression;
        static ::boost::spirit::qi::rule< Iterator,
                                          ComparisonExpression(),
                                          ::boost::spirit::ascii::space_type> bracketedComparisonExpression;
        static void init();
    };
}