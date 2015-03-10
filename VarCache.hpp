//
//  VarCache.hpp
//  lightlang
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include <map>
#include <string>

namespace lightlang {

    struct VarCache 
    {
        /// caches for ints, bools and doubles
        static std::map<std::string, int> intCache;
        static std::map<std::string, bool> boolCache;
        static std::map<std::string, double> doubleCache;
        static std::map<std::string, std::string> stringCache;
    };

}