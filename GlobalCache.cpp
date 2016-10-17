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
        bigCache->setIntArray(key, array);
    }

    void GlobalCache::setValueInIntArray(std::string const & key,
                                         int const index,
                                         int64_t const value)
    {
        bigCache->setValueInIntArray(key, index, value);
    }

    void GlobalCache::pushBackValueInIntArray(std::string const & key,
                                              int64_t const value)
    {
        bigCache->pushBackValueInIntArray(key, value);
    }

    void GlobalCache::setDoubleArray(std::string const & key,
                                     DoubleArray const & array)
    {
        bigCache->setDoubleArray(key, array);
    }

    void GlobalCache::setValueInDoubleArray(std::string const & key,
                                            int const index,
                                            double const value)
    {
        bigCache->setValueInDoubleArray(key, index, value);
    }

    void GlobalCache::pushBackValueInDoubleArray(std::string const & key,
                                                 double const value)
    {
        bigCache->pushBackValueInDoubleArray(key, value);
    }

    void GlobalCache::setByteArray(std::string const & key,
                                   ByteArray const & array)
    {
        bigCache->setByteArray(key, array);
    }

    void GlobalCache::setValueInByteArray(std::string const & key,
                                          int const index,
                                          uint8_t const value)
    {
        bigCache->setValueInByteArray(key, index, value);
    }

    void GlobalCache::pushBackValueInByteArray(std::string const & key,
                                               uint8_t const value)
    {
        bigCache->pushBackValueInByteArray(key, value);
    }
    void GlobalCache::eraseValue(std::string const &key)
    {
        bigCache->eraseValue(key);
    }

    OptionalInt GlobalCache::getInt(std::string const &key)
    {
        return bigCache->getInt(key);
    }

    bool GlobalCache::getInt_(std::string const &key, int64_t &val)
    {
        return bigCache->getInt_(key, val);
    }

    OptionalByte GlobalCache::getByte(std::string const &key)
    {
        return bigCache->getByte(key);
    }

    bool GlobalCache::getByte_(std::string const &key, uint8_t &val)
    {
        return bigCache->getByte_(key, val);
    }

    OptionalDouble GlobalCache::getDouble(std::string const &key)
    {
        return bigCache->getDouble(key);
    }

    bool GlobalCache::getDouble_(std::string const &key, double &val)
    {
        return bigCache->getDouble_(key, val);
    }

    OptionalBool GlobalCache::getBool(std::string const &key)
    {
        return bigCache->getBool(key);
    }

    bool GlobalCache::getBool_(std::string const &key, bool &val)
    {
        return bigCache->getBool_(key, val);
    }

    OptionalString GlobalCache::getString(std::string const &key)
    {
        return bigCache->getString(key);
    }

    bool GlobalCache::getString_(std::string const &key, std::string &val)
    {
        return bigCache->getString_(key, val);
    }

    OptionalValueArray GlobalCache::getList(std::string const &key)
    {
        return bigCache->getList(key);
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
        return bigCache->getIntArray(key);
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
        return bigCache->getDoubleArray(key);
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
        return bigCache->getByteArray(key);
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