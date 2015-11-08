//
//  ScopedVarCache.hpp
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

    using OptionalInt = ::boost::optional<int64_t>;
    using OptionalBool = ::boost::optional<bool>;
    using OptionalDouble = ::boost::optional<double>;
    using OptionalString = ::boost::optional<std::string>;
    using OptionalValueArray = ::boost::optional<ValueArray>;
    using CacheVariant = ::boost::variant<int64_t, bool, double, std::string, ValueArray>;

    /// Represents the type of a cached variable entry
    enum class Type {
        Int, Bool, Double, String, ValueArray
    };

    using OptionalType = ::boost::optional<Type>;

    class ScopedVarCache 
    {
    public:
        /// For representing a variable entry in the 'big cache'
        struct CacheEntry {
            Type type;
            CacheVariant cv;
        };

    private:

        /// caches for ints, bools and doubles, and other types
        std::map<std::string, CacheEntry> m_bigCache;

        /// a separate parameter stack
        std::vector<CacheEntry> m_paramStack;

    public:
        ScopedVarCache();

        /// functions for setting different types
        void setInt(std::string const &key,
                    int64_t const value);
        void setDouble(std::string const &key,
                       double const value);
        void setBool(std::string const &key,
                     bool const value);
        void setString(std::string const &key,
                       std::string const &value);
        void setList(std::string const &key,
                            ValueArray const &value);
        void setTokenInList(std::string const &key,
                            int const index,
                             Value const &value);
        void pushBackTokenInList(std::string const &key,
                                 Value const &value);
        void eraseValue(std::string const &key);

        void resetParamStack();
        
        template <typename V>
        void addToParamStack(Type const type, V const &value)
        {
            CacheEntry ce;
            ce.type = type;
            ce.cv = CacheVariant(value);
            m_paramStack.push_back(ce);
        }

        CacheEntry getParamFromStack(int const i);

        /// functions for getting different types.
        /// These are convenience functions and
        /// shouldn't be used unless it is know values exist.
        OptionalInt getInt(std::string const &key);
        OptionalDouble getDouble(std::string const &key);
        OptionalBool getBool(std::string const &key);
        OptionalString getString(std::string const &key);
        OptionalValueArray getList(std::string const &key);
        Value getListToken(std::string const &key, size_t const index);
        OptionalType getType(std::string const &key);
    };

}