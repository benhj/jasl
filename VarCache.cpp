//
//  VarCache.cpp
//  lightlang
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "VarCache.hpp"

namespace lightlang {

    /// caches for ints, bools and doubles
    std::map<std::string, int> VarCache::intCache;
    std::map<std::string, bool> VarCache::boolCache;
    std::map<std::string, double> VarCache::doubleCache;
    std::map<std::string, std::string> VarCache::stringCache;
    std::string VarCache::script;
 
}