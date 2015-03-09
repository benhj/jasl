#pragma once

#include "Command.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"
#include "expressions/MathExpression.hpp"

namespace lightlang
{

    class MathCommand : public Command
    {

    public:
        MathCommand(Function &func_,
                    OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {
        }

        bool execute() override
        {
            // get the mathmatical operation type (add, sub, exp etc. etc.)
            std::string typeString;
            if (!m_func.getValueA<std::string>(typeString)) {
                m_errorMessage = "math: problem extracting math function type";
                appendToOutput(m_errorMessage);
                return false;
            }

            // see if the operation is a binary one
            if (typeString == "add" ||
                typeString == "div" ||
                typeString == "mult"||
                typeString == "sub") {

                if (!performBinaryOperation(typeString)) {
                    m_errorMessage = "math: problem performing binary operation";
                    appendToOutput(m_errorMessage);
                    return false;
                }

            // or a unary one
            } else if (typeString == "exp" ||
                       typeString == "cos" ||
                       typeString == "sin" ||
                       typeString == "sqrt") {

                if (!performUnaryOperation(typeString)) {
                    m_errorMessage = "math: problem performing unary operation";
                    appendToOutput(m_errorMessage);
                    return false;
                }

            // operation not supported
            } else {
                m_errorMessage = "math: function type not supported";
                appendToOutput(m_errorMessage);
                return false;
            }
            return true;
        }

    private:

        /**
         * @brief add the value to the cache
         * 
         * @param val the value to add to the variable cache
         * @param unaryExpression the expression type
         * @param useDoubleType store value in double or integer cache
         * @return true if successfully set value
         */
        bool setVal(double const val, 
                    bool const unaryExpression = false, 
                    bool const resultIsInteger = false)
        {
            std::string varName;

            // In a unary expression, we'd have something like
            // m_sin(5, a), in which the result of sin(a) is stored in the
            // variable 'a'. This is the third m_func parameter, since we have
            // valueA = 'm_sin' ; valueB = '5'; valueC = 'a'
            // Similarly in a binary expression, e.g. m_add(5, 6, a);
            // valueA = 'm_add'; valueB = '5'; valueC = '6'; valueD = 'a'

            if (unaryExpression ? m_func.getValueC<std::string>(varName) : m_func.getValueD<std::string>(varName)) {

                if (!resultIsInteger) {
                    VarCache::doubleCache[varName] = val;
                } else {
                    VarCache::intCache[varName] = static_cast<int>(val);
                }
                return true;
            } 
            m_errorMessage = "Error storing result in variable with name ";
            m_errorMessage.append(varName);
            appendToOutput(m_errorMessage);
            return false;
        }

        bool doExp(double const val)
        {
            double res = exp(val);
            return setVal(res, true);
        }
        bool doSin(double const val)
        {
            double res = sin(val);
            return setVal(res, true);
        }
        bool doCos(double const val)
        {
            double res = cos(val);
            return setVal(res, true);
        }
        bool doSqrt(double const val)
        {
            double res = sqrt(val);
            return setVal(res, true);
        }

        OptionalDouble extractUnaryValue() const
        {
            double val;
            if (!m_func.getValueB<double>(val)) {
                return OptionalDouble();
            }
            return OptionalDouble(val);
        }

        /// performs a unary operation over a value and stores its result
        /// returns true if successful
        bool processUnaryValueAndStoreResult(std::string const &typeString,
                                             double const val)
        {
            double value;
            if (typeString == "exp") {
                return doExp(val);
            } else if (typeString == "sin") {
                return doSin(val);
            } else if (typeString == "cos") {
                return doCos(val);
            } else if (typeString == "sqrt") {
                return doSqrt(val);
            }
            return false;
        }

        bool performBinaryOperation(std::string const &typeString)
        {
            MathExpression me;
            me.left = m_func.paramB;
            me.right = m_func.paramC;
            me.symbolOperator = typeString;

            double result;
            result = me.evaluate();

            // see if performing the operation broke in some way
            m_errorMessage = me.errorMessage;
            if(!m_errorMessage.empty()) {
                return false;
            }
            
            bool const unaryOperation = false;
            if (!setVal(result, unaryOperation, me.resultIsInteger)) {
                m_errorMessage = "math: problem setting type";
                appendToOutput(m_errorMessage);
                return false;
            }

            return true;
        }

        bool performUnaryOperation(std::string const &typeString)
        {
            // extract the value from the function to perform unary operation over
            auto val = extractUnaryValue();
            if (!val) {
                m_errorMessage = "math: problem extracting unary value";
                appendToOutput(m_errorMessage);
                return false;
            } 

            // Now try and process unary value and store the result
            if (!processUnaryValueAndStoreResult(typeString, *val)) {
                m_errorMessage = "math: problem storing result";
                appendToOutput(m_errorMessage);
                return false;
            }
            
            return true;
        }
    };

}
