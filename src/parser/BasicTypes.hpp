// Ben Jones 2019

#pragma once

#include "caching/Value.hpp"
#include "core/LiteralString.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>
#include <string>

namespace jasl {
    class BasicTypes {
      public:
        using Iterator = std::string::const_iterator;
        
        static ::boost::spirit::qi::rule< Iterator,
                                          double(),
                                          ::boost::spirit::ascii::space_type> doubleRule;
        static ::boost::spirit::qi::rule< Iterator,
                                          int64_t(),
                                          ::boost::spirit::ascii::space_type> intRule;
        static ::boost::spirit::qi::rule< Iterator,
                                          bool(),
                                          ::boost::spirit::ascii::space_type> boolRule;

        static void init();
    };
}