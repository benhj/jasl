//
//  LocalVarCache.hpp
//  jasl
//
//  Created by Ben Jones on 21/03/2015
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Value.hpp"

#include <map>
#include <string>
#include <cstdint>

namespace jasl {

    struct LocalVarCache 
    {
        /// caches for ints, bools and doubles
        std::map<std::string, int64_t> intCache;
        std::map<std::string, bool> boolCache;
        std::map<std::string, double> doubleCache;
        std::map<std::string, std::string> stringCache;
        std::map<std::string, ValueArray> listCache;
    };

}