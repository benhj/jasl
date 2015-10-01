//
//  VarCache.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Value.hpp"

#include <map>
#include <string>
#include <vector>
#include <cstdint>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace jasl {

    typedef ::boost::optional<int64_t> OptionalInt;
    typedef ::boost::optional<bool> OptionalBool;
    typedef ::boost::optional<double> OptionalDouble;
    typedef ::boost::optional<std::string> OptionalString;
    typedef ::boost::optional<ValueArray> OptionalValueArray;
    typedef ::boost::variant<int64_t, bool, double, std::string, ValueArray> CacheVariant;

    struct VarCache 
    {
        /// caches for ints, bools and doubles, and other types
        static std::map<std::string, CacheVariant> bigCache;

        static std::vector<std::string> args;

        /// the script represents the whole program
        static std::string script;

        /// for storing last known error
        static std::string lastKnownError;

        /// functions for setting different types
        static void setInt(std::string const &key,
                           int64_t const value,
                           bool const updateAllowed = true);
        static void setDouble(std::string const &key,
                              double const value,
                              bool const updateAllowed = true);
        static void setBool(std::string const &key,
                            bool const value,
                            bool const updateAllowed = true);
        static void setString(std::string const &key,
                              std::string const &value,
                              bool const updateAllowed = true);
        static void setList(std::string const &key,
                            ValueArray const &value,
                            bool const updateAllowed = true);
        static void setTokenInList(std::string const &key,
                                   int const index,
                                   Value const &value);
        static void eraseValue(std::string const &key);

        /// functions for getting different types.
        /// These are convenience functions and
        /// shouldn't be used unless it is know values exist.
        static OptionalInt getInt(std::string const &key);
        static OptionalDouble getDouble(std::string const &key);
        static OptionalBool getBool(std::string const &key);
        static OptionalString getString(std::string const &key);
        static OptionalValueArray getList(std::string const &key);
        static Value getListToken(std::string const &key, size_t const index);
    };

}