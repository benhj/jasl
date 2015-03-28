//
//  VarCacheManager.hpp
//  jasl
//
//  Created by Ben Jones on 28/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

// Will manage variable cache scoping

#pragma once

#include "SharedVarCache.hpp"
#include "VarExtractor.hpp"
#include <string>

namespace jasl {

    class VarCacheManager
    {

    public:
        VarCacheManager();
        ~VarCacheManager();
        OptionalInt getInt(std::string const &key) const;
        OptionalDouble getDouble(std::string const &key) const;
        OptionalBool getBool(std::string const &key) const;
        OptionalString getString(std::string const &key) const;

    private:

        // Cache specific to this scoping only
        SharedVarCache m_localCache;

        // Cache inherited from parent scope
        SharedVarCache m_inheritedCache;

    };

}