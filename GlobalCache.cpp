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
        bigCache->setInt(key, value);
    }

    void GlobalCache::setDouble(std::string const &key,
                                double const value)
    {
        bigCache->setDouble(key, value);
    }
    void GlobalCache::setBool(std::string const &key,
                              bool const value)
    {
        bigCache->setBool(key, value);
    }
    void GlobalCache::setString(std::string const &key,
                                std::string const &value)
    {
        bigCache->setString(key, value);
    }
    void GlobalCache::setList(std::string const &key,
                              ValueArray const &value)
    {
        bigCache->setList(key, value);
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

    void GlobalCache::eraseValue(std::string const &key)
    {
        bigCache->eraseValue(key);
    }

    OptionalInt GlobalCache::getInt(std::string const &key)
    {
        return bigCache->getInt(key);
    }

    OptionalDouble GlobalCache::getDouble(std::string const &key)
    {
        return bigCache->getDouble(key);
    }

    OptionalBool GlobalCache::getBool(std::string const &key)
    {
        return bigCache->getBool(key);
    }

    OptionalString GlobalCache::getString(std::string const &key)
    {
        return bigCache->getString(key);
    }

    OptionalValueArray GlobalCache::getList(std::string const &key)
    {
        return bigCache->getList(key);
    }

    Value GlobalCache::getListToken(std::string const &key, size_t const index)
    {
        return bigCache->getListToken(key, index);
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