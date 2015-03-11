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
    std::map<std::string, int> VarCache::intCache;
    std::map<std::string, bool> VarCache::boolCache;
    std::map<std::string, double> VarCache::doubleCache;
    std::map<std::string, std::string> VarCache::stringCache;
    std::map<std::string, std::vector<std::string>> VarCache::listCache;
    std::string VarCache::script;
 
}