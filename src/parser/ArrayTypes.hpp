// Ben Jones 2019

#pragma once

#include "core/Function.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>
#include <string>

namespace jasl {
    class ArrayTypes {
      public:
        using Iterator = std::string::const_iterator;
        static ::boost::spirit::qi::rule<Iterator, std::string(), ::boost::spirit::ascii::space_type> arrayLexeme;
        static ::boost::spirit::qi::rule<Iterator, std::string(), ::boost::spirit::ascii::space_type> arrayTypes;
        static ::boost::spirit::qi::rule< Iterator, Function(), ::boost::spirit::ascii::space_type > ints;
        static ::boost::spirit::qi::rule< Iterator, Function(), ::boost::spirit::ascii::space_type > bytes;
        static ::boost::spirit::qi::rule< Iterator, Function(), ::boost::spirit::ascii::space_type > strings;
        static ::boost::spirit::qi::rule< Iterator, Function(), ::boost::spirit::ascii::space_type > bools;
        static ::boost::spirit::qi::rule< Iterator, Function(), ::boost::spirit::ascii::space_type > reals;
        static void init();
    };
}