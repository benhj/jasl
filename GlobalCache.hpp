//
//  GlobalCache.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "Value.hpp"
#include "SharedVarCache.hpp"

#include <boost/optional.hpp>

#include <deque>

namespace jasl {

    struct GlobalCache 
    {

        /// For storing global variables
        static SharedVarCache bigCache;
        static std::vector<std::string> args;

        /// Use a stack to represent function parameters
        static std::deque<ScopedVarCache::CacheEntry> m_paramStack;

        /// the script represents the whole program
        static std::string script;

        /// for storing last known error
        static std::string lastKnownError;

        /// functions for setting different types
        template <typename T> 
        static void setVar(std::string const & key,
                           T const & value,
                           Type const type);

        /// array support
        template <typename V, typename T>
        static void setValueInArray(std::string const & key,
                                    int const index,
                                    V const value);

        template <typename V, typename T>
        static void pushBackValueInArray(std::string const & key,
                                         V const value);
        
        static void setTokenInList(std::string const &key,
                                   int const index,
                                   Value const &value);
        static void pushBackTokenInList(std::string const &key,
                                        Value const &value);

        static void eraseValue(std::string const &key);

        /// functions for getting different types.
        /// These are convenience functions and
        /// shouldn't be used unless it is known values exist.
        template <typename T>
        static ::boost::optional<T> getVar(std::string const & key, Type const type);
        template <typename T>
        static bool getVar_(std::string const & key, T & value, Type const type);
        template <typename T>
        static ::boost::optional<typename T::value_type> 
        getArrayValue(std::string const & key, size_t const index, Type const type);

        static Value getListToken(std::string const &key, size_t const index);
        static OptionalType getType(std::string const &key);

        /// Function parameters handling
        template <typename V>
        static void pushParam(Type const type, V && value);
        static ScopedVarCache::CacheEntry popParam();
    };

}