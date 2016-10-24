//
//  CacheStack.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "CacheStack.hpp"
#include <iostream>

#define SET_VAR(X) \
template void CacheStack::setVar(std::string const & key, X const & value, Type const type);
#define SET_VALUE_IN_ARRAY(V, T) \
template void CacheStack::setValueInArray<V, T>(std::string const & key, int const index, V value);
#define PUSH_BACK_VALUE_IN_ARRAY(V, T) \
template void CacheStack::pushBackValueInArray<V,T>(std::string const & key, V value);
#define GET_VAR(T) \
template ::boost::optional<T> CacheStack::getVar<T>(std::string const&, Type const);
#define GET_VAR_(T) \
template bool CacheStack::getVar_(std::string const & key, T & value, Type const type);
#define GET_ARRAY_VALUE(T) \
template ::boost::optional<typename T::value_type>  CacheStack::getArrayValue<T>(std::string const & key, \
                                                                                 size_t const index, \
                                                                                 Type const type);
namespace jasl {

    CacheStack::CacheStack() 
    : m_cacheStack()
    {
    }

    template <typename T> 
    void CacheStack::setVar(std::string const & key,
                            T const & value,
                            Type const type)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            m_cacheStack[0]->setVar(key, value, type);
            return;
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            if(c->setVarIfExists(key, value, type)) {
                return;
            }
        }

        // Must be here
        m_cacheStack[0]->setVar(key, value, type);
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
 
    void CacheStack::setTokenInList(std::string const &key,
                                    int const index,
                                    Value const &value)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            (void)m_cacheStack[0]->setTokenInList(key, index, value);
            return;
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            if(c->setTokenInList(key, index, value)) {
                return;
            }
        }
    }

    void CacheStack::pushBackTokenInList(std::string const &key,
                                             Value const &value)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            (void)m_cacheStack[0]->pushBackTokenInList(key, value);
            return;
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            if(c->pushBackTokenInList(key, value)) {
                return;
            }
        }
    }

    template <typename V, typename T>
    void CacheStack::setValueInArray(std::string const & key,
                                     int const index,
                                     V const value)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            (void)m_cacheStack[0]->setValueInArray<V, T>(key, index, value);
            return;
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            if(c->setValueInArray<V, T>(key, index, value)) {
                return;
            }
        }
    }

    /// Explicit instantiations
    SET_VALUE_IN_ARRAY(int64_t, IntArray);
    SET_VALUE_IN_ARRAY(double, RealArray);
    SET_VALUE_IN_ARRAY(uint8_t, ByteArray);
    SET_VALUE_IN_ARRAY(std::string, StringArray);

    template <typename V, typename T>
    void CacheStack::pushBackValueInArray(std::string const & key,
                                              V const value)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            (void)m_cacheStack[0]->pushBackValueInArray<V, T>(key, value);
            return;
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            if(c->pushBackValueInArray<V, T>(key, value)) {
                return;
            }
        }
    }

    /// Explicit instantiations
    PUSH_BACK_VALUE_IN_ARRAY(int64_t, IntArray);
    PUSH_BACK_VALUE_IN_ARRAY(double, RealArray);
    PUSH_BACK_VALUE_IN_ARRAY(uint8_t, ByteArray);
    PUSH_BACK_VALUE_IN_ARRAY(std::string, StringArray);

    void CacheStack::eraseValue(std::string const &key)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            (void)m_cacheStack[0]->eraseValue(key);
            return;
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            if(c->eraseValue(key)) {
                return;
            }
        }
    }

    template <typename T>
    ::boost::optional<T> CacheStack::getVar(std::string const & key,
                                            Type const type)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            return m_cacheStack[0]->getVar<T>(key, type);
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            auto opt = c->getVar<T>(key, type);
            if(opt) {
                return opt;
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
    bool CacheStack::getVar_(std::string const & key,
                                 T & val,
                                 Type const type)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            return m_cacheStack[0]->getVar_(key, val, type);
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            if(c->getVar_(key, val, type)) {
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

    Value CacheStack::getListToken(std::string const &key, size_t const index)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            return m_cacheStack[0]->getListToken(key, index);
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            auto opt = c->getVar<List>(key, Type::List);
            if(opt) {
                if(index < opt->size()) {
                    return Value((*opt)[index]); 
                }
            }
        }
        return Value();
    }

    template <typename T>
    ::boost::optional<typename T::value_type> 
    CacheStack::getArrayValue(std::string const & key, 
                              size_t const index,
                              Type const type)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            return m_cacheStack[0]->getArrayValue<T>(key, index, type);
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            auto opt = c->getVar<T>(key, type);
            if(opt) {
                if(index < opt->size()) {
                    return ::boost::optional<typename T::value_type>((*opt)[index]);
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

    OptionalType CacheStack::getType(std::string const &key)
    {
        // if size 1, then always the 0th cache map
        if(m_cacheStack.size() == 1) {
            return m_cacheStack[0]->getType(key);
        }

        // otherwise search through all cache maps
        for (auto const & c : m_cacheStack) {
            auto theType = c->getType(key);
            if(theType) {
                return theType;
            }
        }
        return OptionalType();
    }
}