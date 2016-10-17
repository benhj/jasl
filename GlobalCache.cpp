//
//  GlobalCache.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "GlobalCache.hpp"
#include <iostream>

namespace jasl {

    SharedVarCache GlobalCache::bigCache = std::make_shared<ScopedVarCache>();
    std::vector<std::string> GlobalCache::args;
    std::string GlobalCache::script;
    std::string GlobalCache::lastKnownError;
 
    void GlobalCache::setInt(std::string const &key,
                             int64_t const value)
    {
        bigCache->setVar(key, value, Type::Int);
    }

    void GlobalCache::setByte(std::string const &key,
                              uint8_t const value)
    {
        bigCache->setVar(key, value, Type::Byte);
    }

    void GlobalCache::setDouble(std::string const &key,
                                double const value)
    {
        bigCache->setVar(key, value, Type::Double);
    }
    void GlobalCache::setBool(std::string const &key,
                              bool const value)
    {
        bigCache->setVar(key, value, Type::Bool);
    }
    void GlobalCache::setString(std::string const &key,
                                std::string const &value)
    {
        bigCache->setVar(key, value, Type::String);
    }
    void GlobalCache::setList(std::string const &key,
                              ValueArray const &value)
    {
        bigCache->setVar(key, value, Type::ValueArray);
    }

    void GlobalCache::setTokenInList(std::string const &key,
                                     int const index,
                                     Value const &value)
    {
        bigCache->setTokenInList(key, index, value);
    }

    void GlobalCache::pushBackTokenInList(std::string const &key,
                                          Value const &value)
    {
        bigCache->pushBackTokenInList(key, value);
    }

    void GlobalCache::setIntArray(std::string const & key,
                                  IntArray const & array)
    {
        bigCache->setVar(key, array, Type::IntArray);
    }

    void GlobalCache::setValueInIntArray(std::string const & key,
                                         int const index,
                                         int64_t const value)
    {
        bigCache->setValueInArray<int64_t, IntArray>(key, index, value);
    }

    void GlobalCache::pushBackValueInIntArray(std::string const & key,
                                              int64_t const value)
    {
        bigCache->pushBackValueInArray<int64_t, IntArray>(key, value);
    }

    void GlobalCache::setDoubleArray(std::string const & key,
                                     DoubleArray const & array)
    {
        bigCache->setVar(key, array, Type::DoubleArray);
    }

    void GlobalCache::setValueInDoubleArray(std::string const & key,
                                            int const index,
                                            double const value)
    {
        bigCache->setValueInArray<double, DoubleArray>(key, index, value);
    }

    void GlobalCache::pushBackValueInDoubleArray(std::string const & key,
                                                 double const value)
    {
        bigCache->pushBackValueInArray<double, DoubleArray>(key, value);
    }

    void GlobalCache::setByteArray(std::string const & key,
                                   ByteArray const & array)
    {
        bigCache->setVar(key, array, Type::ByteArray);
    }

    void GlobalCache::setValueInByteArray(std::string const & key,
                                          int const index,
                                          uint8_t const value)
    {
        bigCache->setValueInArray<uint8_t, ByteArray>(key, index, value);
    }

    void GlobalCache::pushBackValueInByteArray(std::string const & key,
                                               uint8_t const value)
    {
        bigCache->pushBackValueInArray<uint8_t, ByteArray>(key, value);
    }
    void GlobalCache::eraseValue(std::string const &key)
    {
        bigCache->eraseValue(key);
    }

    OptionalInt GlobalCache::getInt(std::string const &key)
    {
        return bigCache->getVar<int64_t>(key, Type::Int);
    }

    bool GlobalCache::getInt_(std::string const &key, int64_t &val)
    {
        return bigCache->getInt_(key, val);
    }

    OptionalByte GlobalCache::getByte(std::string const &key)
    {
        return bigCache->getVar<uint8_t>(key, Type::Byte);
    }

    bool GlobalCache::getByte_(std::string const &key, uint8_t &val)
    {
        return bigCache->getByte_(key, val);
    }

    OptionalDouble GlobalCache::getDouble(std::string const &key)
    {
        return bigCache->getVar<double>(key, Type::Double);
    }

    bool GlobalCache::getDouble_(std::string const &key, double &val)
    {
        return bigCache->getDouble_(key, val);
    }

    OptionalBool GlobalCache::getBool(std::string const &key)
    {
        return bigCache->getVar<bool>(key, Type::Bool);
    }

    bool GlobalCache::getBool_(std::string const &key, bool &val)
    {
        return bigCache->getBool_(key, val);
    }

    OptionalString GlobalCache::getString(std::string const &key)
    {
        return bigCache->getVar<std::string>(key, Type::String);
    }

    bool GlobalCache::getString_(std::string const &key, std::string &val)
    {
        return bigCache->getString_(key, val);
    }

    OptionalValueArray GlobalCache::getList(std::string const &key)
    {
        return bigCache->getVar<ValueArray>(key, Type::ValueArray);
    }

    bool GlobalCache::getList_(std::string const &key, ValueArray &val)
    {
        return bigCache->getList_(key, val);
    }

    Value GlobalCache::getListToken(std::string const &key, size_t const index)
    {
        return bigCache->getListToken(key, index);
    }

    OptionalIntArray GlobalCache::getIntArray(std::string const &key)
    {
        return bigCache->getVar<IntArray>(key, Type::IntArray);
    }

    bool GlobalCache::getIntArray_(std::string const &key, IntArray &val)
    {
        return bigCache->getIntArray_(key, val);
    }

    OptionalInt GlobalCache::getIntArrayValue(std::string const &key, size_t const index)
    {
        return bigCache->getIntArrayValue(key, index);
    }

    OptionalDoubleArray GlobalCache::getDoubleArray(std::string const &key)
    {
        return bigCache->getVar<DoubleArray>(key, Type::DoubleArray);
    }

    bool GlobalCache::getDoubleArray_(std::string const &key, DoubleArray &val)
    {
        return bigCache->getDoubleArray_(key, val);
    }

    OptionalDouble GlobalCache::getDoubleArrayValue(std::string const &key, size_t const index)
    {
        return bigCache->getDoubleArrayValue(key, index);
    }

    OptionalByteArray GlobalCache::getByteArray(std::string const &key)
    {
        return bigCache->getVar<ByteArray>(key, Type::ByteArray);
    }

    bool GlobalCache::getByteArray_(std::string const &key, ByteArray &val)
    {
        return bigCache->getByteArray_(key, val);
    }

    OptionalByte GlobalCache::getByteArrayValue(std::string const &key, size_t const index)
    {
        return bigCache->getByteArrayValue(key, index);
    }

    OptionalType GlobalCache::getType(std::string const &key)
    {
        return bigCache->getType(key);
    }

    void GlobalCache::resetParamStack()
    {
        bigCache->resetParamStack();
    }

    ScopedVarCache::CacheEntry GlobalCache::getParamFromStack(int const i)
    {
        return bigCache->getParamFromStack(i);
    }
}