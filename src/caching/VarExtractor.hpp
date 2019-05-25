//
//  VarExtractor.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "caching/Value.hpp"
#include "caching/CacheStack.hpp"
#include "core/ExceptionDispatcher.hpp"
#include "core/LiteralString.hpp"
#include "core/SymbolString.hpp"
#include "commands/expressions/MathExpression.hpp"
#include "commands/expressions/MathByteExpression.hpp"
#include "commands/expressions/ComparisonExpression.hpp"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include <map>
#include <typeinfo>
#include <iostream>
#include <cstdint>
#include <functional>

namespace jasl {

    struct VarExtractor
    {

        template <typename T, typename V>
        static bool tryAnyCast(T & t, V && val)
        {
            try {
                t = simple_any::any_cast<T>(val);
                return true;
            } catch (...) {
                handleException();
            }
            return false;
        }

        static bool tryExtractionDispatch
        (int64_t &t, Value &val)
        {
            return tryAnyCast<int64_t>(t, val);
        }
        static bool tryExtractionDispatch
        (std::vector<int64_t> &t, Value &val)
        {
            return tryAnyCast<std::vector<int64_t>>(t, val);
        }
        static bool tryExtractionDispatch
        (std::vector<double> &t, Value &val)
        {
            return tryAnyCast<std::vector<double>>(t, val);
        }
        static bool tryExtractionDispatch
        (std::vector<uint8_t> &t, Value &val)
        {
            return tryAnyCast<std::vector<uint8_t>>(t, val);
        }
        static bool tryExtractionDispatch
        (std::vector<std::string> &t, Value &val)
        {
            return tryAnyCast<std::vector<std::string>>(t, val);
        }
        static bool tryExtractionDispatch
        (uint8_t &t, Value &val)
        {
            return tryAnyCast<uint8_t>(t, val);
        }
        static bool tryExtractionDispatch
        (char &t, Value &val)
        {
            return tryAnyCast<char>(t, val);
        }
        static bool tryExtractionDispatch
        (bool &t, Value &val)
        {
            return tryAnyCast<bool>(t, val);
        }
        static bool tryExtractionDispatch
        (double &t, Value &val)
        {
            return tryAnyCast<double>(t, val);
        }
        static bool tryExtractionDispatch
        (std::string &t, Value &val)
        {
            return tryAnyCast<std::string>(t, val);
        }
        static bool tryExtractionDispatch
        (List &t, Value &val)
        {
            return tryAnyCast<List>(t, val);
        }
        static bool tryExtractionDispatch
        (MathExpression &t, Value &val)
        {
            return tryAnyCast<MathExpression>(t, val);
        }
        static bool tryExtractionDispatch
        (MathByteExpression &t, Value &val)
        {
            return tryAnyCast<MathByteExpression>(t, val);
        }
        static bool tryExtractionDispatch
        (ComparisonExpression &t, Value &val)
        {
            return tryAnyCast<ComparisonExpression>(t, val);
        }
        static bool tryExtractionDispatch
        (LiteralString &t, Value &val)
        {
            return tryAnyCast<LiteralString>(t, val);
        }
        static bool tryExtractionDispatch
        (SymbolString &t, Value &val)
        {
            return tryAnyCast<SymbolString>(t, val);
        }
        static bool 
        cacheExtract(std::string const & str, int64_t &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::Int);
        }
        static bool 
        cacheExtract(std::string const & str, uint8_t &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::Byte);
        }
        static bool 
        cacheExtract(std::string const & str, char &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::Byte);
        }
        static bool 
        cacheExtract(std::string const & str, bool &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::Bool);
        }
        static bool 
        cacheExtract(std::string const & str, double &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::Real);
        }
        static bool 
        cacheExtract(std::string const & str, std::vector<double> &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::RealArray);
        }
        static bool 
        cacheExtract(std::string const & str, std::vector<uint8_t> &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::ByteArray);
        }
        static bool 
        cacheExtract(std::string const & str, std::vector<std::string> &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::StringArray);
        }
        static bool 
        cacheExtract(std::string const & str, std::vector<int64_t> &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::IntArray);
        }
        static bool 
        cacheExtract(std::string const & str, std::string &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::String);
        }
        static bool 
        cacheExtract(std::string const & str, List &t, SharedCacheStack const &sharedCache)
        {
            return sharedCache->getVar_(str, t, Type::List);
        }

        // The following specializations aren't required, so just return false
        static bool 
        cacheExtract(std::string const &, ComparisonExpression &, SharedCacheStack const &)
        {
            return false;
        }
        static bool 
        cacheExtract(std::string const &, MathExpression &, SharedCacheStack const &)
        {
            return false;
        }
        static bool 
        cacheExtract(std::string const &, MathByteExpression &, SharedCacheStack const &)
        {
            return false;
        }
        static bool 
        cacheExtract(std::string const &, LiteralString &, SharedCacheStack const &)
        {
            return false;
        }

        /// tries to extract a value from val storing the result in t
        template <typename T>
        static bool tryExtraction(T &t, Value &val, SharedCacheStack const &sharedCache)
        {
            // attempt to convert the any in to one of the basic types
            // storing the value in val if successful and returning true
            bool extracted = tryExtractionDispatch(t, val);
            if (extracted) {
                return true;
            } 

            // couldn't extract one of the 'basic types'. This means that the value
            // might be in one of the int, double or bool caches so search for 
            // it therein.
            std::string str;
            extracted = tryAnyCast<std::string>(str, val);
            if (extracted) {
                return cacheExtract(str, t, sharedCache);
            }
            return false;
        }

        /// Following tries to cast to a double. If that fails, then it casts from an
        /// int and then casts the result as a double. Failing both, it tries to
        /// see if its a math expression which it can then evaluate.
        static bool tryToGetAReal(Value &val, double &x, SharedCacheStack const &sharedCache)
        {
            {
                if (tryExtraction<double>(x, val, sharedCache)) {
                    return true;
                }
            }
            {
                int64_t x_i;
                if (tryExtraction<int64_t>(x_i, val, sharedCache)) {
                    x = static_cast<int64_t>(x_i);
                    return true;
                }
            }

            MathExpression me;
            if (tryExtraction<MathExpression>(me, val, sharedCache)) {
                me.sharedCache = sharedCache;
                x = me.evaluate();
                return true;
            }
            return false;
        }

        /// Tries to extract an int. Failing that tries to extract and
        /// evaluate an expression and cast the result to an int
        static bool trySingleIntExtraction(Value &val, int64_t &x, SharedCacheStack const &sharedCache)
        {
            if (tryExtraction<int64_t>(x, val, sharedCache)) {
                return true;
            }

            MathExpression me;
            if (tryExtraction<MathExpression>(me, val, sharedCache)) {
                me.sharedCache = sharedCache;
                x = static_cast<int64_t>(me.evaluate());
                return true;
            }
            return false;
        }

        static bool trySingleByteExtraction(Value &val, uint8_t &x, SharedCacheStack const &sharedCache)
        {
            if (tryExtraction<uint8_t>(x, val, sharedCache)) {
                return true;
            }
            {
                int64_t temp;
                // Can be difficult at the parsing level to distinguish between int and uint
                // therefore pull out int and cast to uint
                if (tryExtraction<int64_t>(temp, val, sharedCache)) {
                    x = (uint8_t)temp;
                    return true;
                }
            }
            {
                // Parsing issue -- detected as a math expression, but
                // we actually need it to be a math byte expression
                MathExpression me;
                if (tryExtraction<MathExpression>(me, val, sharedCache)) {
                    MathByteExpression mbe;
                    mbe.left = me.left;
                    mbe.right = me.right;
                    mbe.symbolOperator = me.symbolOperator;
                    mbe.sharedCache = sharedCache;
                    x = mbe.evaluate();
                    return true;
                }
            }
            {
                MathByteExpression me;
                if (tryExtraction<MathByteExpression>(me, val, sharedCache)) {
                    me.sharedCache = sharedCache;
                    x = me.evaluate();
                    return true;
                }
            }
            return false;
        }

        /// Tries to extract a single int but doesn't bother with math if that doesn't work
        static bool trySingleIntExtractionNoMath(Value &val, int64_t &x, SharedCacheStack const &sharedCache)
        {
            return tryExtraction<int64_t>(x, val, sharedCache);
        }

        /// tries to extract a double from Value storing the result in x
        /// if successful and return true.
        /// Note if extraction of a doulbe is initially unsuccesful it tries
        /// to extract a math expression storing the result of that in x instead
        /// and returning true.
        static bool trySingleRealExtraction(Value &val, double &x, SharedCacheStack const &sharedCache)
        {
            if (tryExtraction<double>(x, val, sharedCache)) {
                return true;
            }

            MathExpression me;
            if (tryExtraction<MathExpression>(me, val, sharedCache)) {
                me.sharedCache = sharedCache;
                x = me.evaluate();
                return true;
            }

            return false;
        }

        /// tries to extract a bool from Value storing the result in x
        /// and returning true if successful. If not initially successful,
        /// tries to extract out a logical expression instead and storing the 
        /// result of that in x and returning true.
        static bool trySingleBoolExtraction(Value &val, bool &x, SharedCacheStack const &sharedCache)
        {
            if (tryExtraction<bool>(x, val, sharedCache)) {
                return true;
            }

            ComparisonExpression ce;
            if (tryExtraction<ComparisonExpression>(ce, val, sharedCache)) {
                ce.m_sharedCache = sharedCache;
                try {
                    x = ce.evaluate();
                    return true;
                } catch (...) {
                    handleException();
                }
            }
            return false;
        }

        static 
        std::function<bool()> trySingleBoolExtraction_V2(Value &val, SharedCacheStack const &sharedCache)
        {
            bool x;
            if (tryExtraction<bool>(x, val, sharedCache)) {
                return [x](){return x;};
            }

            ComparisonExpression ce;
            if (tryExtraction<ComparisonExpression>(ce, val, sharedCache)) {
                ce.m_sharedCache = sharedCache;
                try {
                    return [ce = std::move(ce)](){return ce.evaluate();};
                } catch (...) {
                    handleException();
                }
            }
            return [](){return false;};
        }

        static bool trySingleStringExtraction(Value &val, std::string & x, SharedCacheStack const &sharedCache)
        {
            std::string y;
            if (tryExtraction<std::string>(y, val, sharedCache)) {

                // Note, x will actually be the symbol, not the string
                // that the symbol represents. Therefore, now need to 
                // pull out the variable associated with the symbol
                return sharedCache->getVar_(y, x, Type::String);
            }
            LiteralString ls;
            if (tryExtraction<LiteralString>(ls, val, sharedCache)) {
                x = ls.literal;
                return true;
            }
            return false;
        }

        static bool trySingleListExtraction(Value &val, List & x, SharedCacheStack const &sharedCache)
        {
            if (tryExtraction<List>(x, val, sharedCache)) {
                return true;
            }

            // couldn't extract raw list, see if one is cached 
            std::string varName;
            if (tryExtraction<std::string>(varName, val, sharedCache)) {
                return sharedCache->getVar_(varName, x, Type::List);
            }

            return false;
        }

        template <typename A>
        static bool trySingleArrayExtraction(Value &val, 
                                             A & x, 
                                             SharedCacheStack const &sharedCache,
                                             Type const type)
        {
            if (tryExtraction<A>(x, val, sharedCache)) {
                return true;
            }

            // couldn't extract raw list, see if one is cached 
            std::string varName;
            if (tryExtraction<std::string>(varName, val, sharedCache)) {
                return sharedCache->getVar_(varName, x, type);
            }

            return false;
        }
    };
}
