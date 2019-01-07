//
//  ReverseCommand.cpp
//  jasl
//
//  Created by Ben Jones on 15/11/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "ReverseCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <algorithm>
#include <sstream>

bool jasl::ReverseCommand::m_registered = 
registerCommand<jasl::ReverseCommand>();

namespace jasl
{
    ReverseCommand::ReverseCommand(Function &func_,
                                   SharedCacheStack const &sharedCache,
                                   OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    std::vector<std::string> ReverseCommand::getCommandNames()
    {
        return {"string_reverse"};
    }

    bool ReverseCommand::execute()
    {
        std::string stringName;
        if(!m_func.getValueA<std::string>(stringName, m_sharedCache)) {
            setLastErrorMessage("string_reverse: couldn't parse");
            return false;
        }

        // gracefully fail if string with name cannot be found
        auto found = m_sharedCache->getVar<std::string>(stringName, Type::String);
        if(!found) {
            return false;
        }

        auto result = *found;
        std::reverse(std::begin(result), std::end(result));
        m_sharedCache->setVar(stringName, result, Type::String);
        return true;
    }
}
