//
//  MathExpression.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "caching/Value.hpp"
#include "caching/CacheStack.hpp"
#include <boost/fusion/include/adapt_struct.hpp>
#include <string>

namespace jasl {
    struct MathExpression
    {
        /// the left of the binary expression
        mutable Value left;

        /// the mathmatical operator to be applied
        std::string symbolOperator;

        /// the rhs of the binary expression
        mutable Value right;

        /// will store any generated error that can then be queried
        mutable std::string errorMessage;

        /// is the result an int or a double (implicitly casts)
        mutable bool resultIsInteger;

        /// cache of variables
        SharedCacheStack sharedCache;

        MathExpression();

        /// evaluate the expression
        double evaluate() const;
    };
}

BOOST_FUSION_ADAPT_STRUCT (
    jasl::MathExpression,
    (Value, left)
    (std::string, symbolOperator)
    (Value, right)
)
