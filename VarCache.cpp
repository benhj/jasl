//
//  VarCache.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "VarCache.hpp"
#include <iostream>

namespace jasl {

    /// caches for ints, bools and doubles
    std::map<std::string, CacheVariant> VarCache::bigCache;
    std::vector<std::string> VarCache::args;
    std::string VarCache::script;
    std::string VarCache::lastKnownError;
 
    void VarCache::setInt(std::string const &key,
                          int64_t const value)
    {
        bigCache[key] = CacheVariant((int64_t)value);
    }
    void VarCache::setDouble(std::string const &key,
                             double const value)
    {
        bigCache[key] = CacheVariant((double)value);
    }
    void VarCache::setBool(std::string const &key,
                           bool const value)
    {
        bigCache[key] = CacheVariant((bool)value);
    }
    void VarCache::setString(std::string const &key,
                             std::string const &value)
    {
        bigCache[key] = CacheVariant(value);
    }
    void VarCache::setList(std::string const &key,
                           ValueArray const &value)
    {
        bigCache[key] = CacheVariant(value);
    }

    void VarCache::setTokenInList(std::string const &key,
                                  int const index,
                                  Value const &value)
    {
        auto &keyed = bigCache[key];
        auto &array = ::boost::get<ValueArray>(keyed);
        array[index] = value;
    }

    OptionalInt VarCache::getInt(std::string const &key)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { return ::boost::get<int64_t>(it->second); }
        } catch (...) {}
        return OptionalInt();
    }

    OptionalDouble VarCache::getDouble(std::string const &key)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { return ::boost::get<double>(it->second);  }
        } catch (...) {}
        return OptionalDouble();
    }

    OptionalBool VarCache::getBool(std::string const &key)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { return ::boost::get<bool>(it->second);  }
        } catch (...) {}
        return OptionalBool();
    }

    OptionalString VarCache::getString(std::string const &key)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { return ::boost::get<std::string>(it->second);  }
        } catch (...) {}
        return OptionalString();
    }

    OptionalValueArray VarCache::getList(std::string const &key)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { return ::boost::get<ValueArray>(it->second);  }
        } catch (...) {}
        return OptionalValueArray();
    }

    Value VarCache::getListToken(std::string const &key, size_t const index)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { 
                auto array = ::boost::get<ValueArray>(it->second);  
                if(index < array.size()) {
                    return Value(array[index]); 
                }
            }
        } catch (...) {}
        return Value();
    }


}