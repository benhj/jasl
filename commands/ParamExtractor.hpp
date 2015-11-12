#pragma once

#include "../Function.hpp"
#include "../VarExtractor.hpp"
#include "../GlobalCache.hpp"

namespace jasl 
{

    inline void extractAndUpdateParams(ValueArray & array,
                                       SharedVarCache const &cacheFrom,
                                       SharedVarCache const &cacheTo)
    {
        for(auto & v : array) {

            // Try int
            {
                int64_t value;
                if(VarExtractor::trySingleIntExtraction(v, value, cacheFrom)) {
                    cacheTo->addToParamStack(Type::Int, value);
                    continue;
                }
            }
            // Try double
            {
                double value;
                if(VarExtractor::trySingleDoubleExtraction(v, value, cacheFrom)) {
                    cacheTo->addToParamStack(Type::Double, value);
                    continue;
                }
            } 
            // Try bool
            {
                bool value;
                if(VarExtractor::trySingleBoolExtraction(v, value, cacheFrom)) {
                    cacheTo->addToParamStack(Type::Bool, value);
                    continue;
                }
            } 
            // Try string
            {
                std::string value;
                if(VarExtractor::trySingleStringExtraction(v, value, cacheFrom)) {
                    cacheTo->addToParamStack(Type::String, value);
                    continue;
                }
            } 
            // Try List
            {
                ValueArray value;
                if(VarExtractor::trySingleListExtraction(v, value, cacheFrom)) {
                    cacheTo->addToParamStack(Type::ValueArray, value);
                    continue;
                }
            } 
            // TODO: try list, boolean, string
        }
    }

    inline void extractAndUpdateParams(Value const &val,
                                       SharedVarCache const &cacheTo)
    {

        ValueArray array;
        if(VarExtractor::tryAnyCast(array, val)) {

            auto i = 0;
            for(auto & v : array) {
                std::string symbol;
                VarExtractor::tryAnyCast(symbol, v);  
                auto entry = GlobalCache::getParamFromStack(i);
                if(entry.type == Type::Int) {
                    cacheTo->setInt(symbol, ::boost::get<int64_t>(entry.cv));
                } else if(entry.type == Type::Double) {
                    cacheTo->setDouble(symbol, ::boost::get<double>(entry.cv));
                } else if(entry.type == Type::Bool) {
                    cacheTo->setBool(symbol, ::boost::get<bool>(entry.cv));
                } else if(entry.type == Type::ValueArray) {
                    cacheTo->setList(symbol, ::boost::get<ValueArray>(entry.cv));
                } else if(entry.type == Type::String) {
                    cacheTo->setString(symbol, ::boost::get<std::string>(entry.cv));
                }
                ++i;
            }

            GlobalCache::resetParamStack();
        }

    }

}