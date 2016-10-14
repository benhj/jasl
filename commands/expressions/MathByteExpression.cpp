//
//  MathByteExpression.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "MathByteExpression.hpp"
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

    MathByteExpression::MathByteExpression()
    : errorMessage("")
    {

    }

    uint8_t
    MathByteExpression::evaluate() const
    {
        // Note, for convenience, all math operations are done using doubles
        // The client of this function is responsible for casting to
        // an int if int precision is required
        uint8_t vleft;
        uint8_t vright;

        if(!VarExtractor::trySingleByteExtraction(left, vleft, sharedCache)) {
            errorMessage = "Can't parse left of expression";
        }
        if(!VarExtractor::trySingleByteExtraction(right, vright, sharedCache)) {
            errorMessage = "Can't parse right of expression";
        }

        switch (getMathOp(symbolOperator)) {
            case opadd:  return vleft + vright;
            case opsub:  return vleft - vright;
            case opdiv:  return vleft / vright;
            case opmult: return vleft * vright;
            case opmod:  return vleft % vright;
            case opxor:  return vleft ^ vright;
            default:     throw std::runtime_error("Unknown binary operand");
        }
    }
}