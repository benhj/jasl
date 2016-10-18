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
                if(VarExtractor::trySingleArrayExtraction<ValueArray>(v, value, cacheFrom, Type::ValueArray)) {
                    cacheTo->addToParamStack(Type::ValueArray, value);
                    continue;
                }
            } 
            // Try IntArray
            {
                IntArray value;
                if(VarExtractor::trySingleArrayExtraction<IntArray>(v, value, cacheFrom, Type::IntArray)) {
                    cacheTo->addToParamStack(Type::IntArray, value);
                    continue;
                }
            } 
            // Try DoubleArray
            {
                DoubleArray value;
                if(VarExtractor::trySingleArrayExtraction<DoubleArray>(v, value, cacheFrom, Type::DoubleArray)) {
                    cacheTo->addToParamStack(Type::DoubleArray, value);
                    continue;
                }
            } 
            // Try ByteArray
            {
                ByteArray value;
                if(VarExtractor::trySingleArrayExtraction(v, value, cacheFrom, Type::ByteArray)) {
                    cacheTo->addToParamStack(Type::ByteArray, value);
                    continue;
                }
            } 
            // TODO: try list, bool, string
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
                    cacheTo->setVar(symbol, ::boost::get<int64_t>(entry.cv), Type::Int);
                } else if(entry.type == Type::Double) {
                    cacheTo->setVar(symbol, ::boost::get<double>(entry.cv), Type::Double);
                } else if(entry.type == Type::Bool) {
                    cacheTo->setVar(symbol, ::boost::get<bool>(entry.cv), Type::Bool);
                } else if(entry.type == Type::ValueArray) {
                    cacheTo->setVar(symbol, ::boost::get<ValueArray>(entry.cv), Type::ValueArray);
                } else if(entry.type == Type::IntArray) {
                    cacheTo->setVar(symbol, ::boost::get<IntArray>(entry.cv), Type::IntArray);
                }  else if(entry.type == Type::DoubleArray) {
                    cacheTo->setVar(symbol, ::boost::get<DoubleArray>(entry.cv), Type::DoubleArray);
                }  else if(entry.type == Type::ByteArray) {
                    cacheTo->setVar(symbol, ::boost::get<ByteArray>(entry.cv), Type::ByteArray);
                }  else if(entry.type == Type::String) {
                    cacheTo->setVar(symbol, ::boost::get<std::string>(entry.cv), Type::String);
                } else if(entry.type == Type::Byte) {
                    cacheTo->setVar(symbol, ::boost::get<uint8_t>(entry.cv), Type::Byte);
                }
                ++i;
            }

            GlobalCache::resetParamStack();
        }

    }

}