// Ben Jones 2019

#pragma once

#include "caching/Value.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>

namespace jasl {
    class Parameters {
      public:
        using Iterator = std::string::const_iterator;
        static ::boost::spirit::qi::rule< Iterator, Value(), ::boost::spirit::ascii::space_type> parameter;
        static ::boost::spirit::qi::rule< Iterator, Value(), ::boost::spirit::ascii::space_type> commaParameter;
        static ::boost::spirit::qi::rule< Iterator, Value(), ::boost::spirit::ascii::space_type> parameterList;
        static ::boost::spirit::qi::rule< Iterator, ValueArray(), ::boost::spirit::ascii::space_type> parameters;
        static void init();
    };
}