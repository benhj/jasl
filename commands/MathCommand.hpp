#pragma once

#include "../Function.h"
#include "expressions/MathExpression.hpp"
#include "../VarExtractor.hpp"
#include <string>

namespace lightlang
{

struct MathCommand
{

    MathCommand(Function &func_)
    : func(func_) 
    {

    }

    /**
     * @brief add the value to the cache
     * 
     * @param val the value to add
     * @param single [description]
     * @param useDoubleType [description]
     */
    void setVal(double const val, bool const single = false, bool const useDoubleType = true)
    {
        std::string varName;
        if (single ? func.getValueC<std::string>(varName) : func.getValueD<std::string>(varName)) {

            if (useDoubleType) {
                ParamsTokenizer::ps->paramFloats[varName] = val;
            } else {
                ParamsTokenizer::ps->paramInts[varName] = static_cast<int>(val);
            }
        }
    }

    void doExp(double const val)
    {
        double res = exp(val);
        setVal(res, true);
    }
    void doSin(double const val)
    {
        double res = sin(val);
        setVal(res, true);
    }
    void doCos(double const val)
    {
        double res = cos(val);
        setVal(res, true);
    }
    void doSqrt(double const val)
    {
        double res = sqrt(val);
        setVal(res, true);
    }

    OptionalDouble extractSingleVal() const
    {
        double val;
        if (!func.getValueB<double>(val)) {
            return OptionalDouble();
        }
        return OptionalDouble(val);
    }

    void procSingleVal(std::string const &typeString)
    {
        double value;
        if (typeString == "exp") {
            doExp(val);
        } else if (typeString == "sin") {
            doSin(val);
        } else if (typeString == "cos") {
            doCos(val);
        } else if (typeString == "sqrt") {
            doSqrt(val);
        }
    }

    bool performBinaryOperation(std::string const &typeString)
    {
        MathExpression me;
        me.left = func.paramB;
        me.right = func.paramC;
        me.symbolOperator = typeString;

        double result;

        try {
            result = me.evaluate();
        } catch (...) {
            // todo
        }

        if (!setVal(result, false, useDoubleType)) {
            returnString = "math::problem setting type";
            return false;
        }

        return true;
    }

    bool performUnaryOperation(std::string const &typeString)
    {
        double val;
        if (!extractSingleVal(val)) {
            returnString = "math:: problem extracting value; is it double (cannot be int)?";
            return false;
        } else {
            if (!procSingleVal(typeString, val)) {
                returnString = "math::problem storing result";
                return false;
            }
        }
        return true;
    }

    bool operator()()
    {

        std::string typeString;
        if (!func.getValueA<std::string>(typeString)) {
            returnString = "math::problem extracting math function type";
            return false;
        }

        if (typeString == "add"
            || typeString == "div"
            || typeString == "mult"
            || typeString == "sub") {

            if (!performBinaryOperation(typeString)) {
                return false;
            }

        } else if (typeString == "exp"
                   || typeString == "cos"
                   || typeString == "sin"
                   || typeString == "sqrt") {

            if (!performUnaryOperation(typeString)) {
                return false;
            }

        } else {
            returnString = "math::function type not supported";
            return false;
        }
        return true;

    }

    Function &func;
};

}
