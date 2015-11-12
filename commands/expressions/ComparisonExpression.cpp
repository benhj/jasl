//
//  ComparisonExpression.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "ComparisonExpression.hpp"
#include "../../VarExtractor.hpp"

namespace jasl {

    namespace {
        template <typename T1, typename T2>
        bool doStage2(T1 const valA, 
                      T2 const valB, 
                      std::string const &symbolOperator)
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
        bool doStage1(T const valA, 
                      Value & right, 
                      std::string const &symbolOperator,
                      SharedVarCache const &cache)
        {

            try {

                // where the comparison is between two doubles
                {
                    double d;
                    if (VarExtractor::tryToGetADouble(right, d, cache)) {
                        return doStage2<T, double>(valA, d, symbolOperator);
                    }
                }

                // where the comparison is between two booleans
                bool b;
                if (VarExtractor::trySingleBoolExtraction(right, b, cache)) {
                    return doStage2<T, bool>(valA, b, symbolOperator);
                }
            } catch (...) {
                
            }
            return false;
        }
        
    }

    bool
    ComparisonExpression::evaluate() const
    {
        try {

            // tries to get a double for the first operand
            {
                double d;
                if (VarExtractor::tryToGetADouble(m_left, d, m_sharedCache)) {
                    return doStage1<double>(d, m_right, m_symbolOperator, m_sharedCache);
                }
            }

            // tries to get a boolean for the left operand
            bool b;
            if (VarExtractor::trySingleBoolExtraction(m_left, b, m_sharedCache)) {
                return doStage1<bool>(b, m_right, m_symbolOperator, m_sharedCache);
            }
        } catch (...) {
            
        }
        return false;
    }
}