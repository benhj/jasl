#pragma once

#include <map>
#include <string>

namespace lightland {

    struct VarCache 
    {
        /// caches for ints, bools and doubles
        static std::map<std::string, int> m_ints;
        static std::map<std::string, bool> m_bools;
        static std::map<std::string, double> m_doubles;
    };

}