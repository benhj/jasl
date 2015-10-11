//
//  ScopedVarCache.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "ScopedVarCache.hpp"
#include <iostream>

namespace jasl {

    ScopedVarCache::ScopedVarCache() : m_bigCache()
    {
    }
 
    void ScopedVarCache::setInt(std::string const &key,
                                int64_t const value)
    {
        auto found = m_bigCache.find(key);
        auto proceed = true;

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::Int) {
                proceed = false;
            }
        }
        if(proceed) {
            m_bigCache[key] = { Type::Int, CacheVariant(value) };
        }
    }
    void ScopedVarCache::setDouble(std::string const &key,
                                   double const value)
    {
        auto found = m_bigCache.find(key);
        auto proceed = true;

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::Double) {
                proceed = false;
            }
        }
        if(proceed) {
            m_bigCache[key] = { Type::Double, CacheVariant(value) };
        }
    }
    void ScopedVarCache::setBool(std::string const &key,
                                 bool const value)
    {
        auto found = m_bigCache.find(key);
        auto proceed = true;

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::Bool) {
                proceed = false;
            }
        }
        if(proceed) {
            m_bigCache[key] = { Type::Bool, CacheVariant(value) };
        }
    }
    void ScopedVarCache::setString(std::string const &key,
                                   std::string const &value)
    {
        auto found = m_bigCache.find(key);
        auto proceed = true;

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::String) {
                proceed = false;
            }
        }
        if(proceed) {
            m_bigCache[key] = { Type::String, CacheVariant(value) };
        }
    }
    void ScopedVarCache::setList(std::string const &key,
                                 ValueArray const &value)
    {
        auto found = m_bigCache.find(key);
        auto proceed = true;

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::ValueArray) {
                proceed = false;
            }
        }
        if(proceed) {
            m_bigCache[key] = { Type::ValueArray, CacheVariant(value) };
        }
    }

    void ScopedVarCache::setTokenInList(std::string const &key,
                                        int const index,
                                        Value const &value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<ValueArray>(keyed.cv);
        array[index] = value;
    }

    void ScopedVarCache::pushBackTokenInList(std::string const &key,
                                             Value const &value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<ValueArray>(keyed.cv);
        array.push_back(value);
    }

    void ScopedVarCache::eraseValue(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { m_bigCache.erase(it); }
    }

    OptionalInt ScopedVarCache::getInt(std::string const &key)
    {
        try {
            auto it = m_bigCache.find(key);
            if(it != std::end(m_bigCache)) { 
                return ::boost::get<int64_t>(it->second.cv); 
            }
        } catch (...) {}
        return OptionalInt();
    }

    OptionalDouble ScopedVarCache::getDouble(std::string const &key)
    {
        try {
            auto it = m_bigCache.find(key);
            if(it != std::end(m_bigCache)) { 
                return ::boost::get<double>(it->second.cv);  
            }
        } catch (...) {}
        return OptionalDouble();
    }

    OptionalBool ScopedVarCache::getBool(std::string const &key)
    {
        try {
            auto it = m_bigCache.find(key);
            if(it != std::end(m_bigCache)) { 
                return ::boost::get<bool>(it->second.cv);  
            }
        } catch (...) {}
        return OptionalBool();
    }

    OptionalString ScopedVarCache::getString(std::string const &key)
    {
        try {
            auto it = m_bigCache.find(key);
            if(it != std::end(m_bigCache)) { 
                return ::boost::get<std::string>(it->second.cv);  
            }
        } catch (...) {}
        return OptionalString();
    }

    OptionalValueArray ScopedVarCache::getList(std::string const &key)
    {
        try {
            auto it = m_bigCache.find(key);
            if(it != std::end(m_bigCache)) { 
                return ::boost::get<ValueArray>(it->second.cv);  
            }
        } catch (...) {}
        return OptionalValueArray();
    }

    Value ScopedVarCache::getListToken(std::string const &key, size_t const index)
    {
        try {
            auto it = m_bigCache.find(key);
            if(it != std::end(m_bigCache)) { 
                auto array = ::boost::get<ValueArray>(it->second.cv);  
                if(index < array.size()) {
                    return Value(array[index]); 
                }
            }
        } catch (...) {}
        return Value();
    }

    OptionalType ScopedVarCache::getType(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            return it->second.type;
        }
        return OptionalType();
    }
}