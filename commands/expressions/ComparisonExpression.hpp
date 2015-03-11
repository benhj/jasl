//
//  ComparisonExpression.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "../../Value.hpp"
#include <boost/fusion/include/adapt_struct.hpp>
#include <string>

namespace jasl {
    struct ComparisonExpression
    {
        Value m_left;
        std::string m_symbolOperator;
        Value m_right;
        bool evaluate();
    };
}

BOOST_FUSION_ADAPT_STRUCT (
    jasl::ComparisonExpression,
    (Value, m_left)
    (std::string, m_symbolOperator)
    (Value, m_right)
)

