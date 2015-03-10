//
//  Function.hpp
//  lightlang
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "VarExtractor.hpp"
#include "Value.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <vector>

namespace lightlang
{
    
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    struct Function
    {
        // name of the Function; will always be a string
        std::string name;

        // the function parameters which can be initialized to any type
        Value paramA;
        Value paramB;
        Value paramC;
        Value paramD;
        Value paramE;
        Value paramF;
        Value paramG;

        template <typename Type>
        bool getValueA(Type &t)
        {
            return VarExtractor::tryExtraction(t, paramA);
        }

        template <typename Type>
        bool getValueB(Type &t)
        {
            return VarExtractor::tryExtraction(t, paramB);
        }

        template <typename Type>
        bool getValueC(Type &t)
        {
            return VarExtractor::tryExtraction(t, paramC);
        }

        template <typename Type>
        bool getValueD(Type &t)
        {
            return VarExtractor::tryExtraction(t, paramD);
        }

        template <typename Type>
        bool getValueE(Type &t)
        {
            return VarExtractor::tryExtraction(t, paramE);
        }

        template <typename Type>
        bool getValueF(Type &t)
        {
            return VarExtractor::tryExtraction(t, paramF);
        }

        template <typename Type>
        bool getValueG(Type &t)
        {
            return VarExtractor::tryExtraction(t, paramG);
        }
    };
}

BOOST_FUSION_ADAPT_STRUCT (
    lightlang::Function,
    (std::string, name)
    (Value, paramA)
    (Value, paramB)
    (Value, paramC)
    (Value, paramD)
    (Value, paramE)
    (Value, paramF)
    (Value, paramG)
)
