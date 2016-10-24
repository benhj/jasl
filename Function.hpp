//
//  Function.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "caching/VarExtractor.hpp"
#include "caching/Value.hpp"
#include "caching/CacheStack.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <vector>
#include <memory>

namespace jasl
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
        bool getValueA(Type &t, SharedCacheStack const &sharedCache)
        {
            return VarExtractor::tryExtraction(t, paramA, sharedCache);
        }

        template <typename Type>
        bool getValueB(Type &t, SharedCacheStack const &sharedCache)
        {
            return VarExtractor::tryExtraction(t, paramB, sharedCache);
        }

        template <typename Type>
        bool getValueC(Type &t, SharedCacheStack const &sharedCache)
        {
            return VarExtractor::tryExtraction(t, paramC, sharedCache);
        }

        template <typename Type>
        bool getValueD(Type &t, SharedCacheStack const &sharedCache)
        {
            return VarExtractor::tryExtraction(t, paramD, sharedCache);
        }

        template <typename Type>
        bool getValueE(Type &t, SharedCacheStack const &sharedCache)
        {
            return VarExtractor::tryExtraction(t, paramE, sharedCache);
        }

        template <typename Type>
        bool getValueF(Type &t, SharedCacheStack const &sharedCache)
        {
            return VarExtractor::tryExtraction(t, paramF, sharedCache);
        }

        template <typename Type>
        bool getValueG(Type &t, SharedCacheStack const &sharedCache)
        {
            return VarExtractor::tryExtraction(t, paramG, sharedCache);
        }
    };

    using SharedFunction = std::shared_ptr<Function>;

}

BOOST_FUSION_ADAPT_STRUCT (
    jasl::Function,
    (std::string, name)
    (Value, paramA)
    (Value, paramB)
    (Value, paramC)
    (Value, paramD)
    (Value, paramE)
    (Value, paramF)
    (Value, paramG)
)
