//
//  VarCacheManager.cpp
//  jasl
//
//  Created by Ben Jones on 28/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "VarCacheManager.hpp"

namespace jasl {

    VarCacheManager::VarCacheManager()
    {
    }

    VarCacheManager::~VarCacheManager() 
    {
    }

    OptionalInt 
    VarCacheManager::getInt(std::string const &key) const
    {

        auto it = m_localCache->intCache.find(key);
        if(it != std::end(m_localCache->intCache)) {
            auto itb = m_inheritedCache->intCache.find(key);
            if(itb == std::end(m_inheritedCache->intCache)) {
                return OptionalInt();
            }
            return OptionalInt(itb->second);
        }
        return OptionalInt(it->second);
    }

    OptionalDouble 
    VarCacheManager::getDouble(std::string const &key) const
    {
        auto it = m_localCache->doubleCache.find(key);
        if(it != std::end(m_localCache->doubleCache)) {
            auto itb = m_inheritedCache->doubleCache.find(key);
            if(itb == std::end(m_inheritedCache->doubleCache)) {
                return OptionalDouble();
            }
            return OptionalDouble(itb->second);
        }
        return OptionalDouble(it->second);
    }

    OptionalBool 
    VarCacheManager::getBool(std::string const &key) const
    {
        auto it = m_localCache->boolCache.find(key);
        if(it == std::end(m_localCache->boolCache)) {
            auto itb = m_inheritedCache->boolCache.find(key);
            if(itb == std::end(m_inheritedCache->boolCache)) {
                return OptionalBool();
            }
            return OptionalBool(itb->second);
        }
        return OptionalBool(it->second);
    }

    OptionalString 
    VarCacheManager::getString(std::string const &key) const
    {
        auto it = m_localCache->stringCache.find(key);
        if(it == std::end(m_localCache->stringCache)) {
            auto itb = m_inheritedCache->stringCache.find(key);
            if(itb == std::end(m_inheritedCache->stringCache)) {
                return OptionalString();
            }
            return OptionalString(itb->second);
        }
        return OptionalString(it->second);
    }
}