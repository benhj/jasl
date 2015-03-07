#include "ComparisonExpression.hpp"
#include "VarExtractor.hpp"

namespace lightlang {

    namespace {
        template <typename T1, typename T2>
        bool doStage2(T1 const valA, T2 const valB, string const &symbolOperator)
        {

            if (typeid(T1) != typeid(T2)) {
                throw;
            }

            if (symbolOperator == ">") {
                return valA > valB;
            } else if (symbolOperator == ">=") {
                return valA >= valB;
            } else if (symbolOperator == "<") {
                return valA < valB;
            } else if (symbolOperator == "<=") {
                return valA <= valB;
            } else if (symbolOperator == "==") {
                return valA == valB;
            } else if (symbolOperator == "&&") {
                return valA && valB;
            } else if (symbolOperator == "||") {
                return valA || valB;
            } else if (symbolOperator == "!=") {
                return valA != valB;
            } else {
                throw;
            }
        }

        template <typename T>
        bool doStage1(T const valA, Value & right, string const &symbolOperator)
        {

            try {

                // where the comparison is between two doubles
                {
                    auto result = VarExtractor::tryToGetADouble(right);
                    if (result) {
                        return doStage2<T, double>(valA, *result, symbolOperator);
                    }
                }

                // where the comparison is between two booleans
                auto result = VarExtractor::trySingleBoolExtraction(right);
                if (result) {
                    return doStage2<T, bool>(valA, *result, symbolOperator);
                }
            } catch (...) {
                throw;
            }
        }
    }

    bool
    ComparisonExpression::evaluate()
    {
        try {

            // tries to get a double for the first operand
            {
                auto result = VarExtractor::tryToGetADouble(m_left);
                if (result) {
                    return doStage1<double>(*result, m_right, m_symbolOperator);
                }
            }

            // tries to get a boolean for the left operand
            auto result = arExtractor::trySingleBoolExtraction(m_left);
            if (result) {
                return doStage1<bool>(*result, m_right, m_symbolOperator);
            }
        } catch (...) {
            throw; // what??
        }
    }
}