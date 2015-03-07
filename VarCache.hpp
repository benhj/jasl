#pragma once

#include <map>
#include <string>

namespace lightland {

    struct VarCache 
    {
        /// caches for ints, bools and doubles
        static std::map<std::string, int> intCache;
        static std::map<std::string, bool> boolCache;
        static std::map<std::string, double> doubleCache;
        static std::map<string::string, std::string> stringCache;
    };

}