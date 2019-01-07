
#pragma once
#include "CacheStack.hpp"

namespace {
    // We're in new scope so add a new cache map to the cache stack
    inline jasl::SharedCacheStack withNewCache(jasl::SharedCacheStack const &sharedCache) {
        auto cloned = sharedCache->clone();
        cloned->pushCacheMap();
        return cloned;
    }
}