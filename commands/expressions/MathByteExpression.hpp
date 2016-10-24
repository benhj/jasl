//
//  MathByteExpression.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#pragma once

#include "../../caching/Value.hpp"
#include "../../caching/CacheStack.hpp"
#include <boost/fusion/include/adapt_struct.hpp>
#include <string>

namespace jasl {
    struct MathByteExpression
    {
        /// the left of the binary expression
        mutable Value left;

        /// the mathmatical operator to be applied
        std::string symbolOperator;

        /// the rhs of the binary expression
        mutable Value right;

        /// will store any generated error that can then be queried
        mutable std::string errorMessage;

        /// cache of variables
        SharedCacheStack sharedCache;

        MathByteExpression();

        /// evaluate the expression
        uint8_t evaluate() const;
    };
}

BOOST_FUSION_ADAPT_STRUCT (
    jasl::MathByteExpression,
    (Value, left)
    (std::string, symbolOperator)
    (Value, right)
)
