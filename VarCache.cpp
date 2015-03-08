#include "VarCache.hpp"

namespace lightlang {

    /// caches for ints, bools and doubles
    std::map<std::string, int> VarCache::intCache;
    std::map<std::string, bool> VarCache::boolCache;
    std::map<std::string, double> VarCache::doubleCache;
    std::map<std::string, std::string> VarCache::stringCache;
 
}