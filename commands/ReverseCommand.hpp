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
#include <algorithm>
#include <sstream>

namespace jasl
{
    class ReverseCommand : public Command
    {
    public:
        ReverseCommand(Function &func_,
                       SharedVarCache const &sharedCache = SharedVarCache(),
                       OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, std::move(sharedCache), std::move(output))
        {

        }

        bool execute() override
        {
            std::string stringName;
            if(!m_func.getValueA<std::string>(stringName, m_sharedCache)) {
                setLastErrorMessage("string: couldn't parse");
                return false;
            }

            // gracefully fail if string with name cannot be found
            auto found = m_sharedCache->getString(stringName);
            if(!found) {
                return false;
            }

            auto result = *found;
            std::reverse(std::begin(result), std::end(result));
            m_sharedCache->setString(stringName, result);
            return true;
        }
        
    };

}