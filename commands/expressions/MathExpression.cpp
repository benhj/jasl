#include "MathExpression.hpp"
#include "VarExtractor.hpp"

namespace lightlang {
    double
    MathExpression::evaluate()
    {

        // Note, for convenience, all math operations are done using doubles
        // The client of this function is responsible for casting to
        // an integer if integer precision is required
        double valA;
        double valB;

        if (!VarExtractor::tryToGetADouble(valA, left)) {
            throw;
        }
        if (!VarExtractor::tryToGetADouble(valB, right)) {
            throw;
        }

        if (m_symbolOperator == "+" || m_symbolOperator == "add") {
            return valA + valB;
        } else if (m_symbolOperator == "-" || m_symbolOperator == "sub") {
            return valA - valB;
        } else if (m_symbolOperator == "/" || m_symbolOperator == "div") {
            return valA / valB;
        } else if (m_symbolOperator == "*" || m_symbolOperator == "mult") {
            return valA * valB;
        } else {
            // todo throw
        }
    }
}