//
//  CacheStack.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#pragma once

#include "SharedVarCache.hpp"

#include <deque>
#include <memory>


namespace jasl {

    class CacheStack;
    using SharedCacheStack = std::shared_ptr<CacheStack>;

    class CacheStack
    {

    private:

        /// caches for ints, bools and doubles, and other types
        std::deque<SharedVarCache> m_cacheStack;

    public:
        CacheStack();

        void pushCacheMap();

        /// functions for setting different types
        template <typename T> 
        void setVar(std::string const & key,
                    T const & value,
                    Type const type);
        void setTokenInList(std::string const &key,
                            int const index,
                            Value const &value);
        void pushBackTokenInList(std::string const &key,
                                 Value const &value);

        /// array support
        template <typename V, typename T>
        void setValueInArray(std::string const & key,
                             int const index,
                             V const value);

        template <typename V, typename T>
        void pushBackValueInArray(std::string const & key,
                                  V const value);

        void eraseValue(std::string const &key);

        /// functions for getting different types.
        /// These are convenience functions and
        /// shouldn't be used unless it is known values exist.
        template <typename T>
        ::boost::optional<T> getVar(std::string const & key, Type const type);
        template <typename T>
        bool getVar_(std::string const & key, T & value, Type const type);

        Value getListToken(std::string const &key, size_t const index);
        OptionalType getType(std::string const &key);

        template <typename V>
        ::boost::optional<typename V::value_type> getArrayValue(std::string const & key, 
                                                                size_t const index,
                                                                Type const type);
        
    };

}