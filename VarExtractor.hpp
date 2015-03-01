#pragma once

#include "Value.hpp"
#include "VarCache.hpp"
#include "commands/expressions/MathExpression.h"
#include "commands/expressions/ComparisonExpression.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>

#include <map>
#include <typeinfo>

namespace lightlang {
    struct VarExtractor
    {
        /// searches integer cache for an int with key key storing the
        /// result in output and returning true if successful
        static bool searchInt(int &output, std::string const &key)
        {

            auto it = VarCache::m_ints.find(key);
            if (it != VarCache::m_ints.end()) {
                output = it->second;
                return true;
            } 
            return false;
        }

        /// searches boolean cache for a boolean with key key storing the
        /// result in output and returning true if successful
        static bool searchBool(bool &output, std::string const &key)
        {
            auto it = VarCache::m_bools.find(key);
            if (it != VarCache::m_bools.end()) {
                output = it->second;
                return true;
            }
            return false;
        }

        /// searches double cache for a double with key key storing the
        /// result in output and returning true if successful
        static bool searchDouble(double &output, std::string const &key)
        {
            auto it = VarCache::m_doubles.find(key);
            if (it != VarCache::m_doubles.end()) {
                output = it->second;
                return true;
            } 
            return false;
        }

        template <typename T>
        static bool tryAnyCast(T &t, Value &val)
        {
            try {
                t = boost::any_cast<T>(val);
                return true;
            } catch (...) {
                return false;
            }
        }

        /// trues to extract a value from val storing the result in t
        template <typename T>
        static bool tryExtraction(T &t, Value &val)
        {
            // attempt to convert the any in to one of the basic types
            // storng the value in val if successful and returning true
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
                    extracted = searchInt((int&)t, str);
                } else if (typeid(T) == typeid(bool)) {
                    extracted = searchBool((bool&)t, str);
                } else if (typeid(T) == typeid(double)) {
                    extracted = searchDouble((double&)t, str);
                }
            }
            
            return extracted;
        }

        /// Following tries to cast to a double. If that fails, then it casts from an
        /// int and then casts the result as a double. Failing both, it tries to
        /// see if its a math expression which it can then evaluate.
        static bool tryToGetADouble(double &x, Value &val)
        {
            if (tryExtraction<double>(x, val)) {
                return true;
            }

            int x_;
            if (tryExtraction<int>(x_, val)) {
                x = static_cast<double>(x_);
                return true;
            }

            MathExpression me;
            if (tryExtraction<MathExpression>(me, val)) {
                x = me.evaluate();
                return true;
            }

            return false;
        }

        /// Tries to extract an integer. Failing that tries to extract and
        /// evaluate an expression and cast the result to an int
        static bool trySingleIntExtraction(int &x, Value &val)
        {
            if (tryExtraction<int>(x, val)) {
                return true;
            }

            MathExpression me;
            if (tryExtraction<MathExpression>(me, val)) {

                x = static_cast<int>(me.evaluate());
                return true;
            }

            return false;
        }

        /// tries to extract a double from Value storing the result in x
        /// if successful and return true.
        /// Note if extraction of a doulbe is initially unsuccesful it tries
        /// to extract a math expression storing the result of that in x instead
        /// and returning true.
        static bool trySingleDoubleExtraction(double &x, Value &val)
        {
            if (tryExtraction<double>(x, val)) {
                return true;
            }

            MathExpression me;
            if (tryExtraction<MathExpression>(me, val)) {
                x = me.evaluate();
                return true;
            }

            return false;
        }

        /// tries to extract a boolean from Value storing the result in x
        /// and returning true if successful. If not initially successful,
        /// tries to extract out a logical expression instead and storing the 
        /// result of that in x and returning true.
        static bool trySingleBoolExtraction(bool &x, Value &val)
        {
            if (tryExtraction<bool>(x, val)) {
                return true;
            }

            ComparisonExpression ce;
            if (tryExtraction<ComparisonExpression>(ce, val)) {
                try {
                    x = ce.evaluate();
                } catch (...) {
                    return false;
                }
                return true;
            }

            return false;
        }
    };
}
