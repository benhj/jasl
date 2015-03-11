//
//  SymbolString.hpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <string>

namespace jasl
{
    
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    struct SymbolString
    {
        // the function parameters which can be initialized to any type
        std::string symbol;
    };
}

BOOST_FUSION_ADAPT_STRUCT (
    jasl::SymbolString,
    (std::string, symbol)
)
