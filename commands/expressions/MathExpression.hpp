//
//  MathExpression.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "../../Value.hpp"
#include "../../SharedVarCache.hpp"
#include <boost/fusion/include/adapt_struct.hpp>
#include <string>

namespace jasl {
    struct MathExpression
    {
        /// the left of the binary expression
        Value left;

        /// the mathmatical operator to be applied
        std::string symbolOperator;

        /// the rhs of the binary expression
        Value right;

        /// will store any generated error that can then be queried
        std::string errorMessage;

        /// is the result an integer or a double (implicitly casts)
        bool resultIsInteger;

        /// cache of variables
        SharedVarCache sharedCache;

        MathExpression();

        /// evaluate the expression
        double evaluate();
    };
}

BOOST_FUSION_ADAPT_STRUCT (
    jasl::MathExpression,
    (Value, left)
    (std::string, symbolOperator)
    (Value, right)
)
