// Ben Jones 2019

#pragma once

#include "core/Function.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>

namespace jasl {
    class ListManip {
      public:
        using Iterator = std::string::const_iterator;
        static ::boost::spirit::qi::rule<Iterator, Function(), ::boost::spirit::ascii::space_type> listTokenIndex;
        static ::boost::spirit::qi::rule<Iterator, Function(), ::boost::spirit::ascii::space_type> listGetToken;
        static ::boost::spirit::qi::rule<Iterator, Function(), ::boost::spirit::ascii::space_type> listSetToken;
        static ::boost::spirit::qi::rule<Iterator, Function(), ::boost::spirit::ascii::space_type> listAddToken;
        static ::boost::spirit::qi::rule<Iterator, Function(), ::boost::spirit::ascii::space_type> listMatches;
        static void init();
    };
}