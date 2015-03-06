#pragma once

#include "Value.hpp"
#include <boost/fusion/include/adapt_struct.hpp>
#include <string>

namespace lightlang {
    struct ComparisonExpression
    {
        Value m_left;
        std::string m_symbolOperator;
        Value m_right;
        bool evaluate();
    };
}

BOOST_FUSION_ADAPT_STRUCT (
    lightlang::ComparisonExpression,
    (Value, m_left)
    (std::string, m_symbolOperator)
    (Value, m_right)
)

