//
//  MathExpression.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "MathExpression.hpp"
#include "../../VarExtractor.hpp"
#include <stdexcept>
#include <cstdint>

namespace {

    enum math_op {
        opadd,
        opsub,
        opdiv,
        opmult,
        opmod,
        opxor
    };

    math_op getMathOp(std::string const & symbolOperator) 
    {
        if (symbolOperator == "+" || symbolOperator == "add") {
            return opadd;
        } else if (symbolOperator == "-" || symbolOperator == "sub") {
            return opsub;
        } else if (symbolOperator == "/" || symbolOperator == "div") {
            return opdiv;
        } else if (symbolOperator == "*" || symbolOperator == "mult") {
            return opmult;
        } else if (symbolOperator == "%" || symbolOperator == "mod") {
            return opmod;
        } else if (symbolOperator == "^" || symbolOperator == "xor") {
            return opxor;
        }    
        throw std::runtime_error("Unknown binary operand");
    }
}

namespace jasl {

    MathExpression::MathExpression()
    : errorMessage("")
    , resultIsInteger(true)
    {

    }

    double
    MathExpression::evaluate() const
    {
        // Note, for convenience, all math operations are done using doubles
        // The client of this function is responsible for casting to
        // an int if int precision is required
        double vleft;
        double vright;
        double leftDouble;
        // extracting a double didn't work. Maybe an int
        if(!VarExtractor::trySingleDoubleExtraction(left, leftDouble, sharedCache)) {
            int64_t leftInt;
            if(!VarExtractor::trySingleIntExtraction(left, leftInt, sharedCache)) {
                uint8_t leftChar;
                if(VarExtractor::trySingleByteExtraction(left, leftChar, sharedCache)) {
                    vleft = leftChar;
                } else {
                    errorMessage = "Can't parse left of expression";
                }
            } else {
                vleft = leftInt;
            }
        } else {
            resultIsInteger = false;
            vleft = leftDouble;
        }

        double rightDouble;
        // extracting a double didn't work. Maybe an int
        if(!VarExtractor::trySingleDoubleExtraction(right, rightDouble, sharedCache)) {
            int64_t rightInt;
            if(!VarExtractor::trySingleIntExtraction(right, rightInt, sharedCache)) {
                uint8_t rightChar;
                if(VarExtractor::trySingleByteExtraction(right, rightChar, sharedCache)) {
                    vright = rightChar;
                } else {
                    errorMessage = "Can't parse left of expression";
                }
            } else {
                vright = rightInt;
            }
        } else {
            resultIsInteger = false;
            vright = rightDouble;
        }

        switch (getMathOp(symbolOperator)) {
            case opadd:  return vleft + vright;
            case opsub:  return vleft - vright;
            case opdiv:  return vleft / vright;
            case opmult: return vleft * vright;
            case opmod:  return (int64_t)vleft % (int64_t)vright;
            case opxor:  return (uint8_t)vleft ^ (uint8_t)vright;
            default:     throw std::runtime_error("Unknown binary operand");
        }
    }
}