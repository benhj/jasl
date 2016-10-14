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
            found->second.cv = value;
            return;
        }

        m_bigCache[key] = { Type::Int, value }; 
    }

    void ScopedVarCache::setByte(std::string const &key,
                                 uint8_t const value)
    {
        
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::Byte) {
                return;
            }
            found->second.cv = value;
            return;
        }

        m_bigCache[key] = { Type::Byte, value }; 
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
            found->second.cv = value;
            return;
        }
        m_bigCache[key] = { Type::Double, value };
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
            found->second.cv = value;
            return;
        }
        m_bigCache[key] = { Type::Bool, value};
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
            found->second.cv = value;
            return;
        }
        m_bigCache[key] = { Type::String, value };
        
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
            found->second.cv = value;
            return;
        }
        m_bigCache[key] = { Type::ValueArray, value };
        
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

    void ScopedVarCache::setIntArray(std::string const &key,
                                     IntArray const &value)
    {
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::IntArray) {
                return;
            }
            found->second.cv = value;
            return;
        }
        m_bigCache[key] = { Type::IntArray, value };
        
    }

    void ScopedVarCache::setValueInIntArray(std::string const &key,
                                            int const index,
                                            int64_t const value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<IntArray>(keyed.cv);
        array[index] = value;
    }

    void ScopedVarCache::pushBackValueInIntArray(std::string const &key,
                                                 int64_t const value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<IntArray>(keyed.cv);
        array.push_back(value);
    }

    void ScopedVarCache::setDoubleArray(std::string const &key,
                                        DoubleArray const &value)
    {
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::DoubleArray) {
                return;
            }
            found->second.cv = value;
            return;
        }
        m_bigCache[key] = { Type::DoubleArray, value };
        
    }

    void ScopedVarCache::setValueInDoubleArray(std::string const &key,
                                               int const index,
                                               double const value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<DoubleArray>(keyed.cv);
        array[index] = value;
    }

    void ScopedVarCache::pushBackValueInDoubleArray(std::string const &key,
                                                    double const value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<DoubleArray>(keyed.cv);
        array.push_back(value);
    }

    void ScopedVarCache::setByteArray(std::string const &key,
                                    ByteArray const &value)
    {
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != Type::ByteArray) {
                return;
            }
            found->second.cv = value;
            return;
        }
        m_bigCache[key] = { Type::ByteArray, value };
        
    }

    void ScopedVarCache::setValueInByteArray(std::string const &key,
                                             int const index,
                                             uint8_t const value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<ByteArray>(keyed.cv);
        array[index] = value;
    }

    void ScopedVarCache::pushBackValueInByteArray(std::string const &key,
                                                  uint8_t const value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<ByteArray>(keyed.cv);
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

    OptionalByte ScopedVarCache::getByte(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::Byte) {
                return ::boost::get<uint8_t>(it->second.cv); 
            }
        }
        return OptionalByte();
    }

    bool ScopedVarCache::getByte_(std::string const &key, uint8_t &val)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::Byte) {
                val = ::boost::get<uint8_t>(it->second.cv);  
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

    bool ScopedVarCache::getDouble_(std::string const &key, double &val)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::Double) {
                val = ::boost::get<double>(it->second.cv);  
                return true;
            }
        }
        return false;
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

    bool ScopedVarCache::getBool_(std::string const &key, bool &val)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::Bool) {
                val = ::boost::get<bool>(it->second.cv);  
                return true;
            }
        }
        return false;
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

    bool ScopedVarCache::getString_(std::string const &key, std::string &val)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::String) {
                val = ::boost::get<std::string>(it->second.cv);  
                return true;
            }
        }
        return false;
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

    bool ScopedVarCache::getList_(std::string const &key, ValueArray &val)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::ValueArray) {
                val = ::boost::get<ValueArray>(it->second.cv);  
                return true;
            }
        }
        return false;
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

    OptionalIntArray ScopedVarCache::getIntArray(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::IntArray) {
                return ::boost::get<IntArray>(it->second.cv);  
            }
        }
        return OptionalIntArray();
    }

    bool ScopedVarCache::getIntArray_(std::string const &key, IntArray &val)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::IntArray) {
                val = ::boost::get<IntArray>(it->second.cv);  
                return true;
            }
        }
        return false;
    }

    OptionalInt ScopedVarCache::getIntArrayValue(std::string const &key, size_t const index)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::IntArray) {
                auto array = ::boost::get<IntArray>(it->second.cv);  
                if(index < array.size()) {
                    return OptionalInt(array[index]);
                }
            }
        }
        return OptionalInt();
    }

    OptionalDoubleArray ScopedVarCache::getDoubleArray(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::DoubleArray) {
                return ::boost::get<DoubleArray>(it->second.cv);  
            }
        }
        return OptionalDoubleArray();
    }

    bool ScopedVarCache::getDoubleArray_(std::string const &key, DoubleArray &val)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::DoubleArray) {
                val = ::boost::get<DoubleArray>(it->second.cv);  
                return true;
            }
        }
        return false;
    }

    OptionalDouble ScopedVarCache::getDoubleArrayValue(std::string const &key, size_t const index)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::DoubleArray) {
                auto array = ::boost::get<DoubleArray>(it->second.cv);  
                if(index < array.size()) {
                    return OptionalDouble(array[index]);
                }
            }
        }
        return OptionalDouble();
    }

    OptionalByteArray ScopedVarCache::getByteArray(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::ByteArray) {
                return ::boost::get<ByteArray>(it->second.cv);  
            }
        }
        return OptionalByteArray();
    }

    bool ScopedVarCache::getByteArray_(std::string const &key, ByteArray &val)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::ByteArray) {
                val = ::boost::get<ByteArray>(it->second.cv);  
                return true;
            }
        }
        return false;
    }

    OptionalByte ScopedVarCache::getByteArrayValue(std::string const &key, size_t const index)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::ByteArray) {
                auto array = ::boost::get<ByteArray>(it->second.cv);  
                if(index < array.size()) {
                    return OptionalByte(array[index]);
                }
            }
        }
        return OptionalByte();
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