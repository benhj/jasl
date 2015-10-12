#pragma once

#include "../Function.hpp"
#include "../VarExtractor.hpp"

namespace jasl 
{

    inline void extractAndUpdateParams(ValueArray & array,
                                       SharedVarCache const &cacheFrom,
                                       SharedVarCache const &cacheTo)
    {
        for(auto & v : array) {

            // Try int
            {
                auto d = VarExtractor::trySingleIntExtraction(v, cacheFrom);
                if(d) {
                    cacheTo->addToParamStack(Type::Int, *d);
                    continue;
                }
            }
            // Try double
            {
                auto d = VarExtractor::trySingleDoubleExtraction(v, cacheFrom);
                if(d) {
                    cacheTo->addToParamStack(Type::Double, *d);
                    continue;
                }
            } 
            // Try bool
            {
                auto d = VarExtractor::trySingleBoolExtraction(v, cacheFrom);
                if(d) {
                    cacheTo->addToParamStack(Type::Bool, *d);
                    continue;
                }
            } 
            // Try string
            {
                auto d = VarExtractor::trySingleStringExtraction(v, cacheFrom);
                if(d) {
                    cacheTo->addToParamStack(Type::String, *d);
                    continue;
                }
            } 
            // Try List
            {
                auto d = VarExtractor::trySingleListExtraction(v, cacheFrom);
                if(d) {
                    cacheTo->addToParamStack(Type::ValueArray, *d);
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