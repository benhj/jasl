// Ben Jones 2019

#pragma once

#include "core/Function.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>

namespace jasl {
    class ScreenIO {
      public:
        using Iterator = std::string::const_iterator;
        static ::boost::spirit::qi::rule<Iterator, Function(), ::boost::spirit::ascii::space_type> echo;
        static ::boost::spirit::qi::rule<Iterator, Function(), ::boost::spirit::ascii::space_type> ansiUPRule;
        static void init();
    };
}