// Ben Jones 2019

#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>
#include <string>

namespace jasl {

    class Escapes {
      public:
        using Iterator = std::string::const_iterator;
        static ::boost::spirit::qi::rule< Iterator, std::string() > escapes;
        static void init();
    };
}