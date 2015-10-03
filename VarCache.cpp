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
    std::map<std::string, VarCache::CacheEntry> VarCache::bigCache;
    std::vector<std::string> VarCache::args;
    std::string VarCache::script;
    std::string VarCache::lastKnownError;
 
    void VarCache::setInt(std::string const &key,
                          int64_t const value)
    {
        auto found = bigCache.find(key);
        auto proceed = true;

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(bigCache)) {
            if(found->second.type != Type::Int) {
                proceed = false;
            }
        }
        if(proceed) {
            bigCache[key] = { Type::Int, CacheVariant(value) };
        }
    }
    void VarCache::setDouble(std::string const &key,
                             double const value)
    {
        auto found = bigCache.find(key);
        auto proceed = true;

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(bigCache)) {
            if(found->second.type != Type::Double) {
                proceed = false;
            }
        }
        if(proceed) {
            bigCache[key] = { Type::Double, CacheVariant(value) };
        }
    }
    void VarCache::setBool(std::string const &key,
                           bool const value)
    {
        auto found = bigCache.find(key);
        auto proceed = true;

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(bigCache)) {
            if(found->second.type != Type::Bool) {
                proceed = false;
            }
        }
        if(proceed) {
            bigCache[key] = { Type::Bool, CacheVariant(value) };
        }
    }
    void VarCache::setString(std::string const &key,
                             std::string const &value)
    {
        auto found = bigCache.find(key);
        auto proceed = true;

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(bigCache)) {
            if(found->second.type != Type::String) {
                proceed = false;
            }
        }
        if(proceed) {
            bigCache[key] = { Type::String, CacheVariant(value) };
        }
    }
    void VarCache::setList(std::string const &key,
                           ValueArray const &value)
    {
        auto found = bigCache.find(key);
        auto proceed = true;

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(bigCache)) {
            if(found->second.type != Type::ValueArray) {
                proceed = false;
            }
        }
        if(proceed) {
            bigCache[key] = { Type::ValueArray, CacheVariant(value) };
        }
    }

    void VarCache::setTokenInList(std::string const &key,
                                  int const index,
                                  Value const &value)
    {
        auto &keyed = bigCache[key];
        auto &array = ::boost::get<ValueArray>(keyed.cv);
        array[index] = value;
    }

    void VarCache::eraseValue(std::string const &key)
    {
        auto it = bigCache.find(key);
        if(it != std::end(bigCache)) { bigCache.erase(it); }
    }

    OptionalInt VarCache::getInt(std::string const &key)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { 
                return ::boost::get<int64_t>(it->second.cv); 
            }
        } catch (...) {}
        return OptionalInt();
    }

    OptionalDouble VarCache::getDouble(std::string const &key)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { 
                return ::boost::get<double>(it->second.cv);  
            }
        } catch (...) {}
        return OptionalDouble();
    }

    OptionalBool VarCache::getBool(std::string const &key)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { 
                return ::boost::get<bool>(it->second.cv);  
            }
        } catch (...) {}
        return OptionalBool();
    }

    OptionalString VarCache::getString(std::string const &key)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { 
                return ::boost::get<std::string>(it->second.cv);  
            }
        } catch (...) {}
        return OptionalString();
    }

    OptionalValueArray VarCache::getList(std::string const &key)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { 
                return ::boost::get<ValueArray>(it->second.cv);  
            }
        } catch (...) {}
        return OptionalValueArray();
    }

    Value VarCache::getListToken(std::string const &key, size_t const index)
    {
        try {
            auto it = bigCache.find(key);
            if(it != std::end(bigCache)) { 
                auto array = ::boost::get<ValueArray>(it->second.cv);  
                if(index < array.size()) {
                    return Value(array[index]); 
                }
            }
        } catch (...) {}
        return Value();
    }

    OptionalType VarCache::getType(std::string const &key)
    {
        auto it = bigCache.find(key);
        if(it != std::end(bigCache)) { 
            return it->second.type;
        }
        return OptionalType();
    }
}