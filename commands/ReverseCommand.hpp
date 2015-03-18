//
//  ReverseCommand.cpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"
#include <algorithm>
#include <sstream>

namespace jasl
{
    class ReverseCommand : public Command
    {
    public:
        ReverseCommand(Function &func_,
                       OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {

        }

        bool execute() override
        {
            std::string stringName;
            if(!m_func.getValueA<std::string>(stringName)) {
                setLastErrorMessage("string: couldn't parse");
                return false;
            }

            // gracefully fail if string with name cannot be found
            auto it = VarCache::stringCache.find(stringName);
            if(it == std::end(VarCache::stringCache)) {
                return false;
            }

            std::reverse(std::begin(VarCache::stringCache[stringName]), 
                         std::end(VarCache::stringCache[stringName]));
            return true;
        }
        
    };

}