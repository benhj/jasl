#include "MathExpression.hpp"
#include "../../VarExtractor.hpp"

namespace lightlang {
    double
    MathExpression::evaluate() const
    {

        // Note, for convenience, all math operations are done using doubles
        // The client of this function is responsible for casting to
        // an integer if integer precision is required
        auto valA = VarExtractor::tryToGetADouble(m_left);
        auto valB = VarExtractor::tryToGetADouble(m_right);

        if (!valA || !valB) {
            // TODO -- throw??
        }

        if (m_symbolOperator == "+" || m_symbolOperator == "add") {
            return (*valA) + (*valB);
        } else if (m_symbolOperator == "-" || m_symbolOperator == "sub") {
            return (*valA) - (*valB);
        } else if (m_symbolOperator == "/" || m_symbolOperator == "div") {
            return (*valA) / (*valB);
        } else if (m_symbolOperator == "*" || m_symbolOperator == "mult") {
            return (*valA) * (*valB);
        } else {
            // TODO -- throw??
        }
    }
}