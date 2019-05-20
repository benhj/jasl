//
//  ComparisonExpression.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "ComparisonExpression.hpp"
#include "caching/VarExtractor.hpp"
#include "commands/list/Matches.hpp"

namespace jasl {

    namespace {

        enum logic_code {
            equal,
            notEqual,
            lessThan,
            lessThanEqual,
            greaterThan,
            greaterThanEqual,
            logicOr,
            logicAnd
        };

        logic_code convert(std::string const & symbolOperator) 
        {
            if (symbolOperator == "=" || symbolOperator == "==") {
                return equal;
            } else if(symbolOperator == "!=" || symbolOperator == "/=") {
                return notEqual;
            } else if (symbolOperator == ">") {
                return greaterThan;
            } else if (symbolOperator == ">="){
                return greaterThanEqual;
            } else if (symbolOperator == "<") {
                return lessThan;
            } else if (symbolOperator == "<=") {
                return lessThanEqual;
            } else if (symbolOperator == "&&") {
                return logicAnd;
            } else if (symbolOperator == "||") {
                return logicOr;
            } 
            throw;
        }

        bool doStage2(std::string const valA, 
                      std::string const valB, 
                      std::string const &symbolOperator)
        {
            switch (convert(symbolOperator)) {
                case equal: return valA == valB;
                case notEqual: return valA != valB;
                case lessThan:  return valA < valB;
                case lessThanEqual:  return valA <= valB;
                case greaterThan:  return valA > valB;
                case greaterThanEqual: return valA >= valB;
                default: throw;
            }
        }

        template <typename T1, typename T2>
        bool doStage2(T1 const valA, 
                      T2 const valB, 
                      std::string const &symbolOperator)
        {
            if (typeid(T1) != typeid(T2)) {
                throw;
            }
            switch (convert(symbolOperator)) {
                case equal: return valA == valB;
                case notEqual: return valA != valB;
                case lessThan:  return valA < valB;
                case lessThanEqual:  return valA <= valB;
                case greaterThan:  return valA > valB;
                case greaterThanEqual: return valA >= valB;
                case logicOr:  return valA || valB;
                case logicAnd:  return valA && valB;
                default: throw;
            }
        }

        bool doStage1(std::string const valA, 
                      Value & right, 
                      std::string const &symbolOperator,
                      SharedCacheStack const &cache)
        {

            try {
                // strings
                std::string str;
                if (VarExtractor::trySingleStringExtraction(right, str, cache)) {
                    return doStage2(valA, str, symbolOperator);;
                }
            } catch (...) {
                
            }
            return false;
        }

        template <typename T>
        bool doStage1(T const valA, 
                      Value & right, 
                      std::string const &symbolOperator,
                      SharedCacheStack const &cache)
        {

            try {

                // where the comparison is between two doubles
                {
                    double d;
                    if (VarExtractor::tryToGetAReal(right, d, cache)) {
                        return doStage2<T, double>(valA, d, symbolOperator);
                    }
                }

                // where the comparison is between two bools
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
                if (VarExtractor::tryToGetAReal(m_left, d, m_sharedCache)) {
                    return doStage1<double>(d, m_right, m_symbolOperator, m_sharedCache);
                }
            }

            // tries to get a bool for the left operand
            bool b;
            if (VarExtractor::trySingleBoolExtraction(m_left, b, m_sharedCache)) {
                return doStage1<bool>(b, m_right, m_symbolOperator, m_sharedCache);
            }

            // strings
            std::string str;
            if (VarExtractor::trySingleStringExtraction(m_left, str, m_sharedCache)) {
                return doStage1(str, m_right, m_symbolOperator, m_sharedCache);
            }

            // lists
            List list;
            List list2;
            if (VarExtractor::trySingleListExtraction(m_left, list, m_sharedCache) &&
                VarExtractor::trySingleListExtraction(m_right, list2, m_sharedCache)) {
                return Matches(m_sharedCache).matches(list, list2);
            }

        } catch (...) {
            
        }
        return false;
    }
}