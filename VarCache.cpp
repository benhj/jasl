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

    int64_t VarCache::getInt(std::string const &key)
    {
        return intCache[key];
    }

    double VarCache::getDouble(std::string const &key)
    {
        return doubleCache[key];
    }

    bool VarCache::getBool(std::string const &key)
    {
        return boolCache[key];
    }

    std::string VarCache::getString(std::string const &key)
    {
        return stringCache[key];
    }

    ValueArray VarCache::getList(std::string const &key)
    {
        return listCache[key];
    }


}