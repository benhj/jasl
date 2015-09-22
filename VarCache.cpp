//
//  VarCache.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "VarCache.hpp"

namespace jasl {

    /// caches for ints, bools and doubles
    std::map<std::string, int64_t> VarCache::intCache;
    std::map<std::string, bool> VarCache::boolCache;
    std::map<std::string, double> VarCache::doubleCache;
    std::map<std::string, std::string> VarCache::stringCache;
    std::map<std::string, ValueArray> VarCache::listCache;
    std::vector<std::string> VarCache::args;
    std::string VarCache::script;
    std::string VarCache::lastKnownError;
 
    void VarCache::setInt(std::string const &key,
                          int64_t const value)
    {
        intCache[key] = value;
    }
    void VarCache::setDouble(std::string const &key,
                             double const value)
    {
        doubleCache[key] = value;
    }
    void VarCache::setBool(std::string const &key,
                           bool const value)
    {
        boolCache[key] = value;
    }
    void VarCache::setString(std::string const &key,
                             std::string const &value)
    {
        stringCache[key] = value;
    }
    void VarCache::setList(std::string const &key,
                           ValueArray const &value)
    {
        listCache[key] = value;
    }

    void VarCache::setTokenInList(std::string const &key,
                                  int const index,
                                  Value const &value)
    {
        listCache[key][index] = value;
    }

    OptionalInt VarCache::getInt(std::string const &key)
    {
        auto it = intCache.find(key);
        if(it != std::end(intCache)) { return it->second; }
        return OptionalInt();
    }

    OptionalDouble VarCache::getDouble(std::string const &key)
    {
        auto it = doubleCache.find(key);
        if(it != std::end(doubleCache)) { return it->second; }
        return OptionalDouble();
    }

    OptionalBool VarCache::getBool(std::string const &key)
    {
        auto it = boolCache.find(key);
        if(it != std::end(boolCache))  { return it->second; }
        return OptionalBool();
    }

    OptionalString VarCache::getString(std::string const &key)
    {
        auto it = stringCache.find(key);
        if(it != std::end(stringCache))  { return it->second; }
        return OptionalString();
    }

    OptionalValueArray VarCache::getList(std::string const &key)
    {
        auto it = listCache.find(key);
        if(it != std::end(listCache))  { return it->second; }
        return OptionalValueArray();
    }


}