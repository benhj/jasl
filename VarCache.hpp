//
//  VarCache.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstdint>

namespace jasl {

    struct VarCache 
    {
        /// caches for ints, bools and doubles
        static std::map<std::string, int64_t> intCache;
        static std::map<std::string, bool> boolCache;
        static std::map<std::string, double> doubleCache;
        static std::map<std::string, std::string> stringCache;
        static std::map<std::string, std::vector<std::string>> listCache;
        static std::vector<std::string> args;

        /// the script represents the whole program
        static std::string script;
    };

}