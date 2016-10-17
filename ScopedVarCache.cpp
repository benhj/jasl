//
//  ScopedVarCache.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
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

    template <typename T> 
    void ScopedVarCache::setVar(std::string const & key,
                                T const & value,
                                Type const type)
    {
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != type) {
                return;
            }
            found->second.cv = value;
            return;
        }

        m_bigCache[key] = { type, value }; 
    }

    /// Explicit template instantiations (--linkage issues).
    template void ScopedVarCache::setVar(std::string const & key, int64_t const & value, Type const type);
    template void ScopedVarCache::setVar(std::string const & key, uint8_t const & value, Type const type);
    template void ScopedVarCache::setVar(std::string const & key, std::string const & value, Type const type);
    template void ScopedVarCache::setVar(std::string const & key, ValueArray const & value, Type const type);
    template void ScopedVarCache::setVar(std::string const & key, IntArray const & value, Type const type);
    template void ScopedVarCache::setVar(std::string const & key, DoubleArray const & value, Type const type);
    template void ScopedVarCache::setVar(std::string const & key, ByteArray const & value, Type const type);
    template void ScopedVarCache::setVar(std::string const & key, bool const & value, Type const type);
    template void ScopedVarCache::setVar(std::string const & key, double const & value, Type const type);
 
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

    template <typename V, typename T>
    void ScopedVarCache::setValueInArray(std::string const & key,
                                         int const index,
                                         V const value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<T>(keyed.cv);
        array[index] = value;
    }

    /// Explicit instantiations
    template void 
    ScopedVarCache::setValueInArray<int64_t, IntArray>
    (std::string const & key, int const index, int64_t value);
    template void 
    ScopedVarCache::setValueInArray<double, DoubleArray>
    (std::string const & key, int const index, double value);
    template void 
    ScopedVarCache::setValueInArray<uint8_t, ByteArray>
    (std::string const & key, int const index, uint8_t value);

    template <typename V, typename T>
    void ScopedVarCache::pushBackValueInArray(std::string const & key,
                                              V const value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<T>(keyed.cv);
        array.push_back(value);
    }

    /// Explicit instantiations
    template void 
    ScopedVarCache::pushBackValueInArray<int64_t, IntArray>
    (std::string const & key, int64_t value);
    template void 
    ScopedVarCache::pushBackValueInArray<double, DoubleArray>
    (std::string const & key, double value);
    template void 
    ScopedVarCache::pushBackValueInArray<uint8_t, ByteArray>
    (std::string const & key, uint8_t value);

    void ScopedVarCache::eraseValue(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { m_bigCache.erase(it); }
    }

    template <typename T>
    ::boost::optional<T> ScopedVarCache::getVar(std::string const & key,
                                                Type const type)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == type) {
                return ::boost::get<T>(it->second.cv); 
            }
        }
        return ::boost::optional<T>();
    }

    /// Explicit instantiations
    template ::boost::optional<int64_t> 
    ScopedVarCache::getVar<int64_t>(std::string const&, Type const);
    template ::boost::optional<double> 
    ScopedVarCache::getVar<double>(std::string const&, Type const);
    template ::boost::optional<bool> 
    ScopedVarCache::getVar<bool>(std::string const&, Type const);
    template ::boost::optional<uint8_t> 
    ScopedVarCache::getVar<uint8_t>(std::string const&, Type const);
    template ::boost::optional<ValueArray> 
    ScopedVarCache::getVar<ValueArray>(std::string const&, Type const);
    template ::boost::optional<IntArray> 
    ScopedVarCache::getVar<IntArray>(std::string const&, Type const);
    template ::boost::optional<DoubleArray> 
    ScopedVarCache::getVar<DoubleArray>(std::string const&, Type const);
    template ::boost::optional<ByteArray> 
    ScopedVarCache::getVar<ByteArray>(std::string const&, Type const);
    template ::boost::optional<std::string> 
    ScopedVarCache::getVar<std::string>(std::string const&, Type const);

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