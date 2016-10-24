#pragma once

#include "../Function.hpp"
#include "../VarExtractor.hpp"
#include "../GlobalCache.hpp"

namespace jasl 
{

    inline void pushParams(List & array, SharedVarCache const &cacheFrom)
    {
        for(auto & v : array) {

            // Try int
            {
                int64_t value;
                if(VarExtractor::trySingleIntExtraction(v, value, cacheFrom)) {
                    GlobalCache::pushParam(Type::Int, (int64_t)value);
                    continue;
                }
            }
            // Try double
            {
                double value;
                if(VarExtractor::trySingleRealExtraction(v, value, cacheFrom)) {
                    GlobalCache::pushParam(Type::Real, value);
                    continue;
                }
            } 
            // Try bool
            {
                bool value;
                if(VarExtractor::trySingleBoolExtraction(v, value, cacheFrom)) {
                    GlobalCache::pushParam(Type::Bool, value);
                    continue;
                }
            } 
            // Try byte
            {
                uint8_t value;
                if(VarExtractor::trySingleByteExtraction(v, value, cacheFrom)) {
                    GlobalCache::pushParam(Type::Byte, (uint8_t)value);
                    continue;
                }
            } 
            // Try string
            {
                std::string value;
                if(VarExtractor::trySingleStringExtraction(v, value, cacheFrom)) {
                    GlobalCache::pushParam(Type::String, value);
                    continue;
                }
            } 
            // Try List
            {
                List value;
                if(VarExtractor::trySingleArrayExtraction<List>(v, value, cacheFrom, Type::List)) {
                    GlobalCache::pushParam(Type::List, value);
                    continue;
                }
            } 
            // Try IntArray
            {
                IntArray value;
                if(VarExtractor::trySingleArrayExtraction<IntArray>(v, value, cacheFrom, Type::IntArray)) {
                    GlobalCache::pushParam(Type::IntArray, value);
                    continue;
                }
            } 
            // Try RealArray
            {
                RealArray value;
                if(VarExtractor::trySingleArrayExtraction<RealArray>(v, value, cacheFrom, Type::RealArray)) {
                    GlobalCache::pushParam(Type::RealArray, value);
                    continue;
                }
            } 
            // Try ByteArray
            {
                ByteArray value;
                if(VarExtractor::trySingleArrayExtraction(v, value, cacheFrom, Type::ByteArray)) {
                    GlobalCache::pushParam(Type::ByteArray, value);
                    continue;
                }
            } 
            // TODO: try list, bool, string
        }
    }

    inline void popParams(Value const &val, SharedVarCache const &cacheTo)
    {

        ValueArray array;
        if(VarExtractor::tryAnyCast(array, val)) {

            for(auto & v : array) {
                std::string symbol;
                VarExtractor::tryAnyCast(symbol, v);  
                auto entry = GlobalCache::popParam();
                if(entry.type == Type::Int) {
                    cacheTo->setVar(symbol, ::boost::get<int64_t>(entry.cv), Type::Int);
                } else if(entry.type == Type::Real) {
                    cacheTo->setVar(symbol, ::boost::get<double>(entry.cv), Type::Real);
                } else if(entry.type == Type::Bool) {
                    cacheTo->setVar(symbol, ::boost::get<bool>(entry.cv), Type::Bool);
                } else if(entry.type == Type::List) {
                    cacheTo->setVar(symbol, ::boost::get<List>(entry.cv), Type::List);
                } else if(entry.type == Type::IntArray) {
                    cacheTo->setVar(symbol, ::boost::get<IntArray>(entry.cv), Type::IntArray);
                }  else if(entry.type == Type::RealArray) {
                    cacheTo->setVar(symbol, ::boost::get<RealArray>(entry.cv), Type::RealArray);
                }  else if(entry.type == Type::ByteArray) {
                    cacheTo->setVar(symbol, ::boost::get<ByteArray>(entry.cv), Type::ByteArray);
                }  else if(entry.type == Type::String) {
                    cacheTo->setVar(symbol, ::boost::get<std::string>(entry.cv), Type::String);
                } else if(entry.type == Type::Byte) {
                    cacheTo->setVar(symbol, ::boost::get<uint8_t>(entry.cv), Type::Byte);
                }
            }
        }

    }

}