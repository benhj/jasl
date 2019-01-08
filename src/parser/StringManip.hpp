// Ben Jones 2019

#pragma once

#include "core/Function.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>

namespace jasl {
    class StringManip {
      public:
        using Iterator = std::string::const_iterator;
        static ::boost::spirit::qi::rule<Iterator, Function(), ::boost::spirit::ascii::space_type> appendRule;
        static ::boost::spirit::qi::rule<Iterator, Function(), ::boost::spirit::ascii::space_type> reverseRule;
        static ::boost::spirit::qi::rule<Iterator, Function(), ::boost::spirit::ascii::space_type> concatRule;
        static void init();
    };
}