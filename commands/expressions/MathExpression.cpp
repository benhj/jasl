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
        double valA;
        // extracting a double didn't work. Maybe an int
        if(!VarExtractor::trySingleDoubleExtraction(left, valA, sharedCache)) {
            int64_t valC;
            if(VarExtractor::trySingleIntExtraction(left, valC, sharedCache)) {
                vleft = valC;
            } else {
                errorMessage = "Can't parse left of expression";
            }
        } else {
            resultIsInteger = false;
            vleft = valA;
        }

        double valB;
        // extracting a double didn't work. Maybe an int
        if(!VarExtractor::trySingleDoubleExtraction(right, valB, sharedCache)) {
            int64_t valD;
            if(VarExtractor::trySingleIntExtraction(right, valD, sharedCache)) {
                vright = valD;
            } else {
                errorMessage = "Can't parse right of expression";
            }
        } else {
            resultIsInteger = false;
            vright = valB;
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