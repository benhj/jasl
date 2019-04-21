/// (c) 2019 Ben H. D. Jones

#pragma once

#include "caching/CacheStack.hpp"
#include "caching/Value.hpp"

namespace jasl {
    bool matches(List const & first,
                 List const & second,
                 SharedCacheStack const & sharedCache);
}