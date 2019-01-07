// Ben Jones 2019

#pragma once

#include "core/LiteralString.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>
#include <string>

namespace jasl {
    class Strings {
      public:
        using Iterator = std::string::const_iterator;
        
        static ::boost::spirit::qi::rule< Iterator, 
                                          std::string(),
                                          ::boost::spirit::ascii::space_type> genericString;

        static ::boost::spirit::qi::rule< Iterator, 
                                          LiteralString(), 
                                          ::boost::spirit::ascii::space_type> quotedString;
        static ::boost::spirit::qi::rule< Iterator, 
                                          LiteralString(), 
                                          ::boost::spirit::ascii::space_type> doubleQuotedString;
        static void init();
    };
}