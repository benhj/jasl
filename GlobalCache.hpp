//
//  GlobalCache.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Value.hpp"
#include "SharedVarCache.hpp"

#include <boost/optional.hpp>

namespace jasl {

    struct GlobalCache 
    {

        /// For storing global variables
        static SharedVarCache bigCache;

        static std::vector<std::string> args;

        /// the script represents the whole program
        static std::string script;

        /// for storing last known error
        static std::string lastKnownError;

        /// functions for setting different types
        static void setInt(std::string const &key,
                           int64_t const value);
        static void setDouble(std::string const &key,
                              double const value);
        static void setBool(std::string const &key,
                            bool const value);
        static void setString(std::string const &key,
                              std::string const &value);
        static void setList(std::string const &key,
                            ValueArray const &value);
        static void setTokenInList(std::string const &key,
                                   int const index,
                                   Value const &value);
        static void pushBackTokenInList(std::string const &key,
                                        Value const &value);

        /// int array support
        static void setIntArray(std::string const & key,
                                IntArray const & array);
        static void setValueInIntArray(std::string const & key,
                                       int const index,
                                       int64_t const value);
        static void pushBackValueInIntArray(std::string const & key,
                                            int64_t const value);

        // double array support
        static void setDoubleArray(std::string const & key,
                                   DoubleArray const & array);
        static void setValueInDoubleArray(std::string const & key,
                                          int const index,
                                          double const value);
        static void pushBackValueInDoubleArray(std::string const & key,
                                               double const value);


        static void eraseValue(std::string const &key);

        static void resetParamStack();

        template <typename V>
        static void addToParamStack(Type const type, V const &value)
        {
            bigCache->addToParamStack(type, value);
        }

        static ScopedVarCache::CacheEntry getParamFromStack(int const i);

        /// functions for getting different types.
        /// These are convenience functions and
        /// shouldn't be used unless it is know values exist.
        static OptionalInt getInt(std::string const &key);
        static bool getInt_(std::string const &key, int64_t &val);
        static OptionalDouble getDouble(std::string const &key);
        static bool getDouble_(std::string const &key, double &val);
        static OptionalBool getBool(std::string const &key);
        static bool getBool_(std::string const &key, bool &val);
        static OptionalString getString(std::string const &key);
        static bool getString_(std::string const &key, std::string &val);
        static OptionalValueArray getList(std::string const &key);
        static bool getList_(std::string const &key, ValueArray &val);
        static Value getListToken(std::string const &key, size_t const index);

        static OptionalIntArray getIntArray(std::string const &key);
        static bool getIntArray_(std::string const &key, IntArray &val);
        static OptionalInt getIntArrayValue(std::string const &key, size_t const index);

        static OptionalDoubleArray getDoubleArray(std::string const &key);
        static bool getDoubleArray_(std::string const &key, DoubleArray &val);
        static OptionalDouble getDoubleArrayValue(std::string const &key, size_t const index);

        static OptionalType getType(std::string const &key);
    };

}