//
//  VarExtractor.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Value.hpp"
#include "VarCache.hpp"
#include "LiteralString.hpp"
#include "SymbolString.hpp"
#include "commands/expressions/MathExpression.hpp"
#include "commands/expressions/ComparisonExpression.hpp"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>
#include <boost/optional.hpp>

#include <map>
#include <typeinfo>
#include <iostream>

namespace jasl {

    typedef ::boost::optional<int> OptionalInt;
    typedef ::boost::optional<bool> OptionalBool;
    typedef ::boost::optional<double> OptionalDouble;
    typedef ::boost::optional<std::string> OptionalString;

    struct VarExtractor
    {
        /// searches integer cache for an int with key key storing the
        /// result in output and returning true if successful
        static OptionalInt searchInt(std::string const &key)
        {
            auto it = VarCache::intCache.find(key);
            if (it != VarCache::intCache.end()) {
                return OptionalInt(it->second);
            } 
            return OptionalInt();
        }

        /// searches boolean cache for a boolean with key key storing the
        /// result in output and returning true if successful
        static OptionalBool searchBool(std::string const &key)
        {
            auto it = VarCache::boolCache.find(key);
            if (it != VarCache::boolCache.end()) {
                return OptionalBool(it->second);
            }
            return OptionalBool();
        }

        /// searches double cache for a double with key key storing the
        /// result in output and returning true if successful
        static OptionalDouble searchDouble(std::string const &key)
        {
            auto it = VarCache::doubleCache.find(key);
            if (it != VarCache::doubleCache.end()) {
                return OptionalDouble(it->second);
            } 
            return OptionalDouble();
        }

        /// searches string cache for a string with key key storing the
        /// result in output and returning true if successful
        static OptionalString searchString(std::string const &key)
        {
            auto it = VarCache::stringCache.find(key);
            if (it != VarCache::stringCache.end()) {
                return OptionalString(it->second);
            } 
            return OptionalString();
        }

        template <typename T>
        static bool tryAnyCast(T &t, Value &val)
        {
            try {
                t = boost::any_cast<T>(val);
                return true;
            } catch (...) {
            }
            return false;
        }

        /// tries to extract a value from val storing the result in t
        template <typename T>
        static bool tryExtraction(T &t, Value &val)
        {
            // attempt to convert the any in to one of the basic types
            // storing the value in val if successful and returning true
            bool extracted = false;
            if (typeid(T) == typeid(int)) {
                extracted = tryAnyCast<int>((int&)t, val);
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
            // might be in one of the integer, double or boolean caches so search for 
            // it therein.
            std::string str;
            extracted = tryAnyCast<std::string>(str, val);
            if (extracted) {
                if (typeid(T) == typeid(int)) {
                    auto result = searchInt(str);
                    if(result) {
                        (int&)t = *result;
                        return true;
                    }
                } else if (typeid(T) == typeid(bool)) {
                    auto result = searchBool(str);
                    if(result) {
                        (bool&)t = *result;
                        return true;
                    }
                } else if (typeid(T) == typeid(double)) {
                    auto result = searchDouble(str);
                    if(result) {
                        (double&)t = *result;
                        return true;
                    }
                }
            }
            return false;
        }

        /// Following tries to cast to a double. If that fails, then it casts from an
        /// int and then casts the result as a double. Failing both, it tries to
        /// see if its a math expression which it can then evaluate.
        static OptionalDouble tryToGetADouble(Value &val)
        {

            {
                double x;
                if (tryExtraction<double>(x, val)) {
                    return OptionalDouble(x);
                }
            }
            {
                int x;
                if (tryExtraction<int>(x, val)) {
                    return OptionalDouble(x);
                }
            }

            MathExpression me;
            if (tryExtraction<MathExpression>(me, val)) {
                return OptionalDouble(me.evaluate());
            }

            return OptionalDouble();
        }

        /// Tries to extract an integer. Failing that tries to extract and
        /// evaluate an expression and cast the result to an int
        static OptionalInt trySingleIntExtraction(Value &val)
        {
            int x;
            if (tryExtraction<int>(x, val)) {
                return OptionalInt(x);
            }

            MathExpression me;
            if (tryExtraction<MathExpression>(me, val)) {

                return OptionalInt(static_cast<int>(me.evaluate()));
            }

            return OptionalInt();
        }

        /// tries to extract a double from Value storing the result in x
        /// if successful and return true.
        /// Note if extraction of a doulbe is initially unsuccesful it tries
        /// to extract a math expression storing the result of that in x instead
        /// and returning true.
        static OptionalDouble trySingleDoubleExtraction(Value &val)
        {
            double x;
            if (tryExtraction<double>(x, val)) {
                return OptionalDouble(x);
            }

            MathExpression me;
            if (tryExtraction<MathExpression>(me, val)) {
                return OptionalDouble(me.evaluate());
            }

            return OptionalDouble();
        }

        /// tries to extract a boolean from Value storing the result in x
        /// and returning true if successful. If not initially successful,
        /// tries to extract out a logical expression instead and storing the 
        /// result of that in x and returning true.
        static OptionalBool trySingleBoolExtraction(Value &val)
        {
            bool x;
            if (tryExtraction<bool>(x, val)) {
                return OptionalBool(x);
            }

            ComparisonExpression ce;
            if (tryExtraction<ComparisonExpression>(ce, val)) {
                try {
                    return OptionalBool(ce.evaluate());
                } catch (...) {
                    
                }
            }

            return OptionalBool();
        }
    };
}
