//
//  ScopedVarCache.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "ScopedVarCache.hpp"
#include <iostream>

#define SET_VAR(X) \
template void ScopedVarCache::setVar(std::string const & key, X const & value, Type const type);
#define SET_VALUE_IN_ARRAY(V, T) \
template void ScopedVarCache::setValueInArray<V, T>(std::string const & key, int const index, V value);
#define PUSH_BACK_VALUE_IN_ARRAY(V, T) \
template void ScopedVarCache::pushBackValueInArray<V,T>(std::string const & key, V value);
#define GET_VAR(T) \
template ::boost::optional<T> ScopedVarCache::getVar<T>(std::string const&, Type const);
#define GET_VAR_(T) \
template bool ScopedVarCache::getVar_(std::string const & key, T & value, Type const type);
#define GET_ARRAY_VALUE(T) \
template ::boost::optional<typename T::value_type>  ScopedVarCache::getArrayValue<T>(std::string const & key, \
                                                                                     size_t const index, \
                                                                                     Type const type);

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
    SET_VAR(int64_t);
    SET_VAR(uint8_t);
    SET_VAR(std::string);
    SET_VAR(ValueArray);
    SET_VAR(IntArray);
    SET_VAR(DoubleArray);
    SET_VAR(ByteArray);
    SET_VAR(bool);
    SET_VAR(double);
 
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
    SET_VALUE_IN_ARRAY(int64_t, IntArray);
    SET_VALUE_IN_ARRAY(double, DoubleArray);
    SET_VALUE_IN_ARRAY(uint8_t, ByteArray);

    template <typename V, typename T>
    void ScopedVarCache::pushBackValueInArray(std::string const & key,
                                              V const value)
    {
        auto &keyed = m_bigCache[key];
        auto &array = ::boost::get<T>(keyed.cv);
        array.push_back(value);
    }

    /// Explicit instantiations
    PUSH_BACK_VALUE_IN_ARRAY(int64_t, IntArray);
    PUSH_BACK_VALUE_IN_ARRAY(double, DoubleArray);
    PUSH_BACK_VALUE_IN_ARRAY(uint8_t, ByteArray);

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
    GET_VAR(int64_t);
    GET_VAR(double);
    GET_VAR(bool);
    GET_VAR(uint8_t);
    GET_VAR(ValueArray);
    GET_VAR(IntArray);
    GET_VAR(DoubleArray);
    GET_VAR(ByteArray);
    GET_VAR(std::string);

    template <typename T>
    bool ScopedVarCache::getVar_(std::string const & key,
                                 T & val,
                                 Type const type)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == type) {
                val = ::boost::get<T>(it->second.cv);  
                return true;
            }
        }
        return false;
    }

    /// Explicit template instantiations (--linkage issues).
    GET_VAR_(int64_t);
    GET_VAR_(uint8_t);
    GET_VAR_(std::string);
    GET_VAR_(ValueArray);
    GET_VAR_(IntArray);
    GET_VAR_(DoubleArray);
    GET_VAR_(ByteArray);
    GET_VAR_(bool);
    GET_VAR_(double);

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

    template <typename T>
    ::boost::optional<typename T::value_type> 
    ScopedVarCache::getArrayValue(std::string const & key, 
                                  size_t const index,
                                  Type const type)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == type) {
                auto array = ::boost::get<T>(it->second.cv);  
                if(index < array.size()) {
                    return ::boost::optional<typename T::value_type>(array[index]);
                }
            }
        }
        return ::boost::optional<typename T::value_type>();
    }

    /// Explicit instantiations
    GET_ARRAY_VALUE(IntArray);
    GET_ARRAY_VALUE(DoubleArray);
    GET_ARRAY_VALUE(ByteArray);

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