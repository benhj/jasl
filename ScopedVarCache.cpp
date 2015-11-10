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

    template <typename T>
    auto getVType(T const & t) -> T {
        return ::boost::get<T>(t);
    }
    
    ScopedVarCache::ScopedVarCache() 
    : m_bigCache()
    , m_paramStack()
    {
    }
 
    void ScopedVarCache::setInt(std::string const &key,
                                int64_t const value)
    {
        
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::Int) {
                return;
            }
            found->second.cv = std::move(value);
            return;
        }

        m_bigCache[key] = { Type::Int, std::move(value) };
        
    }
    void ScopedVarCache::setDouble(std::string const &key,
                                   double const value)
    {
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::Double) {
                return;
            }
            found->second.cv = std::move(value);
            return;
        }
        m_bigCache[key] = { Type::Double, std::move(value) };
    }
    void ScopedVarCache::setBool(std::string const &key,
                                 bool const value)
    {
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::Bool) {
                return;
            }
            found->second.cv = std::move(value);
            return;
        }
        m_bigCache[key] = { Type::Bool, std::move(value)};
    }
    void ScopedVarCache::setString(std::string const &key,
                                   std::string const &value)
    {
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::String) {
                return;
            }
            found->second.cv = std::move(value);
            return;
        }
        m_bigCache[key] = { Type::String, std::move(value) };
        
    }
    void ScopedVarCache::setList(std::string const &key,
                                 ValueArray const &value)
    {
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::ValueArray) {
                return;
            }
            found->second.cv = std::move(value);
            return;
        }
        m_bigCache[key] = { Type::ValueArray, std::move(value) };
        
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
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::Int) {
                return ::boost::get<int64_t>(it->second.cv); 
            }
        }
        return OptionalInt();
    }

    bool ScopedVarCache::getInt_(std::string const &key, int64_t &val)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::Int) {
                val = ::boost::get<int64_t>(it->second.cv);  
                return true;
            }
        }
        return false;
    }

    OptionalDouble ScopedVarCache::getDouble(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::Double) {
                return ::boost::get<double>(it->second.cv);  
            }
        }
        return OptionalDouble();
    }

    OptionalBool ScopedVarCache::getBool(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::Bool) {
                return ::boost::get<bool>(it->second.cv);  
            }
        }
        return OptionalBool();
    }

    OptionalString ScopedVarCache::getString(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::String) {
                return ::boost::get<std::string>(it->second.cv);  
            }
        }
        return OptionalString();
    }

    OptionalValueArray ScopedVarCache::getList(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::ValueArray) {
                return ::boost::get<ValueArray>(it->second.cv);  
            }
        }
        return OptionalValueArray();
    }

    Value ScopedVarCache::getListToken(std::string const &key, size_t const index)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::ValueArray) {
                auto array = ::boost::get<ValueArray>(it->second.cv);  
                if(index < array.size()) {
                    return Value(array[index]); 
                }
            }
        }
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

    void ScopedVarCache::resetParamStack()
    {
        decltype(m_paramStack)().swap(m_paramStack);
    }

    ScopedVarCache::CacheEntry ScopedVarCache::getParamFromStack(int const i)
    {
        return m_paramStack[i];
    }
}