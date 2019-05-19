//
//  ScopedVarCache.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "ScopedVarCache.hpp"
#include <iostream>

#define SET_VAR(X) \
template void ScopedVarCache::setVar(std::string const & key, X const & value, Type const type);
#define SET_VAR_IF_EXISTS(X) \
template bool ScopedVarCache::setVarIfExists(std::string const & key, X const & value, Type const type);
#define SET_VALUE_IN_ARRAY(V, T) \
template bool ScopedVarCache::setValueInArray<V, T>(std::string const & key, int const index, V value);
#define PUSH_BACK_VALUE_IN_ARRAY(V, T) \
template bool ScopedVarCache::pushBackValueInArray<V,T>(std::string const & key, V value);
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
    SET_VAR(List);
    SET_VAR(IntArray);
    SET_VAR(RealArray);
    SET_VAR(ByteArray);
    SET_VAR(StringArray);
    SET_VAR(bool);
    SET_VAR(double);

    template <typename T> 
    bool ScopedVarCache::setVarIfExists(std::string const & key,
                                        T const & value,
                                        Type const type)
    {
        auto found = m_bigCache.find(key);

        // If the variable already exists, then only
        // proceed in updating it, if the type is correct
        if(found != std::end(m_bigCache)) {
            if(found->second.type != type) {
                return false;
            }
            found->second.cv = value;
            return true;
        }
        return false;
    }

    /// Explicit template instantiations (--linkage issues).
    SET_VAR_IF_EXISTS(int64_t);
    SET_VAR_IF_EXISTS(uint8_t);
    SET_VAR_IF_EXISTS(std::string);
    SET_VAR_IF_EXISTS(List);
    SET_VAR_IF_EXISTS(IntArray);
    SET_VAR_IF_EXISTS(RealArray);
    SET_VAR_IF_EXISTS(ByteArray);
    SET_VAR_IF_EXISTS(StringArray);
    SET_VAR_IF_EXISTS(bool);
    SET_VAR_IF_EXISTS(double);
 
    bool ScopedVarCache::setTokenInList(std::string const &key,
                                        int const index,
                                        Value const &value)
    {
        auto found = m_bigCache.find(key);
        if(found != std::end(m_bigCache)) {
            auto &keyed = found->second;
            auto &array = ::boost::get<List>(keyed.cv);
            array[index] = value;
            return true;
        }
        return false;
    }

    bool ScopedVarCache::pushBackTokenInList(std::string const &key,
                                             Value const &value)
    {
        auto found = m_bigCache.find(key);
        if(found != std::end(m_bigCache)) {
            auto &keyed = found->second;
            auto &array = ::boost::get<List>(keyed.cv);
            array.push_back(value);
            return true;
        }
        return false;
    }

    template <typename V, typename T>
    bool ScopedVarCache::setValueInArray(std::string const & key,
                                         int const index,
                                         V const value)
    {
        auto found = m_bigCache.find(key);
        if(found != std::end(m_bigCache)) {
            auto &keyed = found->second;
            auto &array = ::boost::get<T>(keyed.cv);
            array[index] = value;
            return true;
        }
        return false;
    }

    /// Explicit instantiations
    SET_VALUE_IN_ARRAY(int64_t, IntArray);
    SET_VALUE_IN_ARRAY(double, RealArray);
    SET_VALUE_IN_ARRAY(uint8_t, ByteArray);
    SET_VALUE_IN_ARRAY(std::string, StringArray);
    SET_VALUE_IN_ARRAY(Value, List);

    template <typename V, typename T>
    bool ScopedVarCache::pushBackValueInArray(std::string const & key,
                                              V const value)
    {
        auto found = m_bigCache.find(key);
        if(found != std::end(m_bigCache)) {
            auto &keyed = found->second;
            auto &array = ::boost::get<T>(keyed.cv);
            array.push_back(value);
            return true;
        }
        return false;
    }

    /// Explicit instantiations
    PUSH_BACK_VALUE_IN_ARRAY(int64_t, IntArray);
    PUSH_BACK_VALUE_IN_ARRAY(double, RealArray);
    PUSH_BACK_VALUE_IN_ARRAY(uint8_t, ByteArray);
    PUSH_BACK_VALUE_IN_ARRAY(std::string, StringArray);
    PUSH_BACK_VALUE_IN_ARRAY(Value, List);

    bool ScopedVarCache::eraseValue(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            m_bigCache.erase(it); 
            return true;
        }
        return false;
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
    GET_VAR(List);
    GET_VAR(IntArray);
    GET_VAR(RealArray);
    GET_VAR(ByteArray);
    GET_VAR(StringArray);
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
    GET_VAR_(List);
    GET_VAR_(IntArray);
    GET_VAR_(RealArray);
    GET_VAR_(ByteArray);
    GET_VAR_(StringArray);
    GET_VAR_(bool);
    GET_VAR_(double);

    Value ScopedVarCache::getListToken(std::string const &key, size_t const index)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            if(it->second.type == Type::List) {
                auto array = ::boost::get<List>(it->second.cv);  
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
    GET_ARRAY_VALUE(RealArray);
    GET_ARRAY_VALUE(ByteArray);
    GET_ARRAY_VALUE(StringArray);
    GET_ARRAY_VALUE(List);

    OptionalType ScopedVarCache::getType(std::string const &key)
    {
        auto it = m_bigCache.find(key);
        if(it != std::end(m_bigCache)) { 
            return it->second.type;
        }
        return OptionalType();
    }
}