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
                double aDouble;
                bool notUsed;
                if (VarExtractor::tryToGetADouble(aDouble, right, notUsed)) {
                    return doStage2<T, double>(valA, aDouble, symbolOperator);
                }

                // where the comparison is between two booleans
                bool aBool;
                if (VarExtractor::trySingleBoolExtraction(aBool, right)) {
                    return doStage2<T, bool>(valA, aBool, symbolOperator);
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
            double aDouble;
            bool notUsed;
            if (VarExtractor::tryToGetADouble(aDouble, m_left, notUsed)) {
                return doStage1<double>(aDouble, m_right, m_symbolOperator);
            }

            // tries to get a boolean for the left operand
            bool aBool;
            if (VarExtractor::trySingleBoolExtraction(aBool, m_left)) {
                return doStage1<bool>(aBool, m_right, m_symbolOperator);
            }
        } catch (...) {

            throw; // what??

        }

    }

}