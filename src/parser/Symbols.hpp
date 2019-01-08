// Ben Jones 2019

#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>
#include <string>

namespace jasl {
    class Symbols {
      public:
        using Iterator = std::string::const_iterator;
        static ::boost::spirit::qi::rule< Iterator, 
                                          std::string(),
                                          ::boost::spirit::ascii::space_type > mathSymbols;
        static ::boost::spirit::qi::rule< Iterator,
                                          std::string(),
                                          ::boost::spirit::ascii::space_type > comparisonSymbols;
        static void init();
    };
}