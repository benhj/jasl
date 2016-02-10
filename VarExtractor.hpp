//
//  VarExtractor.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Value.hpp"
#include "SharedVarCache.hpp"
#include "LiteralString.hpp"
#include "SymbolString.hpp"
#include "commands/expressions/MathExpression.hpp"
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
            }
            return false;
        }

        /// tries to extract a value from val storing the result in t
        template <typename T>
        static bool tryExtraction(T &t, Value &val, SharedVarCache const &sharedCache)
        {
            // attempt to convert the any in to one of the basic types
            // storing the value in val if successful and returning true
            bool extracted = false;
            if (typeid(T) == typeid(int64_t)) {
                extracted = tryAnyCast<int64_t>((int64_t&)t, val);
            } else if (typeid(T) == typeid(bool)) {
                extracted = tryAnyCast<bool>((bool&)t, val);
            } else if (typeid(T) == typeid(double)) {
                extracted = tryAnyCast<double>((double&)t, val);
            } else if (typeid(T) == typeid(std::string)) {
                extracted = tryAnyCast<std::string>((std::string&)t, val);
            } else if (typeid(T) == typeid(ValueArray)) {
                extracted = tryAnyCast<ValueArray>((ValueArray&)t, val);
            } else if (typeid(T) == typeid(MathExpression)) {
                extracted = tryAnyCast<MathExpression>((MathExpression&)t, val);
            } else if (typeid(T) == typeid(ComparisonExpression)) {
                extracted = tryAnyCast<ComparisonExpression>((ComparisonExpression&)t, val);
            } else if (typeid(T) == typeid(LiteralString)) {
                extracted = tryAnyCast<LiteralString>((LiteralString&)t, val);
            } else if (typeid(T) == typeid(SymbolString)) {
                extracted = tryAnyCast<SymbolString>((SymbolString&)t, val);
            }
            if (extracted) {
                return true;
            } 

            // couldn't extract one of the 'basic types'. This means that the value
            // might be in one of the int, double or bool caches so search for 
            // it therein.
            std::string str;
            extracted = tryAnyCast<std::string>(str, val);
            if (extracted) {
                if (typeid(T) == typeid(int64_t)) {
                    auto result = sharedCache->getInt_(str, (int64_t&)t);
                    if(result) {
                        return true;
                    }
                } else if (typeid(T) == typeid(bool)) {
                    auto result = sharedCache->getBool_(str, (bool&)t);
                    if(result) {
                        return true;
                    }
                } else if (typeid(T) == typeid(double)) {
                    auto result = sharedCache->getDouble_(str, (double&)t);
                    if(result) {
                        return true;
                    }
                }
            }
            return false;
        }

        /// Following tries to cast to a double. If that fails, then it casts from an
        /// int and then casts the result as a double. Failing both, it tries to
        /// see if its a math expression which it can then evaluate.
        static bool tryToGetADouble(Value &val, double &x, SharedVarCache const &sharedCache)
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
        static bool trySingleIntExtraction(Value &val, int64_t &x, SharedVarCache const &sharedCache)
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

        /// Tries to extract a single int but doesn't bother with math if that doesn't work
        static bool trySingleIntExtractionNoMath(Value &val, int64_t &x, SharedVarCache const &sharedCache)
        {
            return tryExtraction<int64_t>(x, val, sharedCache);
        }

        /// tries to extract a double from Value storing the result in x
        /// if successful and return true.
        /// Note if extraction of a doulbe is initially unsuccesful it tries
        /// to extract a math expression storing the result of that in x instead
        /// and returning true.
        static bool trySingleDoubleExtraction(Value &val, double &x, SharedVarCache const &sharedCache)
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
        static bool trySingleBoolExtraction(Value &val, bool &x, SharedVarCache const &sharedCache)
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
                    
                }
            }
            return false;
        }

        static 
        std::function<bool()> trySingleBoolExtraction_V2(Value &val, SharedVarCache const &sharedCache)
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
                    
                }
            }
            return [](){return false;};
        }

        static bool trySingleStringExtraction(Value &val, std::string & x, SharedVarCache const &sharedCache)
        {
            std::string y;
            if (tryExtraction<std::string>(y, val, sharedCache)) {

                // Note, x will actually be the symbol, not the string
                // that the symbol represents. Therefore, now need to 
                // pull out the variable associated with the symbol
                return sharedCache->getString_(y, x);
            }
            LiteralString ls;
            if (tryExtraction<LiteralString>(ls, val, sharedCache)) {
                x = ls.literal;
                return true;
            }
            return false;
        }

        static bool trySingleListExtraction(Value &val, ValueArray & x, SharedVarCache const &sharedCache)
        {
            if (tryExtraction<ValueArray>(x, val, sharedCache)) {
                return true;
            }

            // couldn't extract raw list, see if one is cached 
            std::string varName;
            if (tryExtraction<std::string>(varName, val, sharedCache)) {
                return sharedCache->getList_(varName, x);
            }

            return false;
        }
    };
}
