#pragma once

#include "Value.hpp"
#include <boost/fusion/include/adapt_struct.hpp>
#include <string>

namespace lightlang {
    struct MathExpression
    {
        Value m_left;
        std::string m_symbolOperator;
        Value m_right;
        double evaluate(bool &isDouble);
    };
}

BOOST_FUSION_ADAPT_STRUCT (
    MathExpression,
    (Value, m_left)
    (std::string, m_symbolOperator)
    (Value, m_right)
)
