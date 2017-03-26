//
//  ComparisonExpression.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "caching/Value.hpp"
#include "caching/CacheStack.hpp"
#include <boost/fusion/include/adapt_struct.hpp>
#include <string>

namespace jasl {
    struct ComparisonExpression
    {
        mutable Value m_left;
        std::string m_symbolOperator;
        mutable Value m_right;
        SharedCacheStack m_sharedCache;
        bool evaluate() const;
    };
}

BOOST_FUSION_ADAPT_STRUCT (
    jasl::ComparisonExpression,
    (Value, m_left)
    (std::string, m_symbolOperator)
    (Value, m_right)
)

