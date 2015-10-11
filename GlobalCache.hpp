//
//  GlobalCache.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Value.hpp"
#include "ScopedVarCache.hpp"

#include <boost/optional.hpp>

namespace jasl {

    struct GlobalCache 
    {

        /// For storing global variables
        static ScopedVarCache bigCache;

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
        static OptionalType getType(std::string const &key);
    };

}