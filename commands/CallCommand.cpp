//
//  CallCommand.hpp
//  jasl
//
//  Created by Ben Jones on on 10/03/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "CallCommand.hpp"
#include "../CommandInterpretor.hpp"
#include <vector>

namespace jasl 
{
    CallCommand::CallCommand(Function &func_, OptionalOutputStream const &output)
    : Command(func_, output)
    {
    }

    bool CallCommand::execute()
    {
        std::string functionName; 
        (void)m_func.getValueA<std::string>(functionName);
        return searchForFunction(functionName);
    }

    bool CallCommand::searchForFunction(std::string &functionName)
    {
        // First get all subroutines
        std::string &scriptString = VarCache::script;
        CommandInterpretor ci;
        auto functions = ci.parseStringCollection(scriptString);

        // Find all functions that are block functions
        decltype(functions) matched;
        for(auto &f : functions) {
            std::string funcName;
            if (f.name.find("block") != std::string::npos) {
                matched.push_back(f);
            }
        }

        // Second, search for block with given name
        auto it = std::find_if(std::begin(matched), std::end(matched), [&](Function &f) { 
            std::string name;
            (void)f.getValueA<std::string>(name);
            return name == functionName;
        });

        // block function was found so execute it
        if (it != std::end(matched)) {
            return parseCommand(*it);
        } 
        return false;
    }

    bool CallCommand::parseCommand(Function &function)
    {
        CommandInterpretor ci;
        (void)ci.interpretFunc(function, m_outputStream);
        return true;
    }
}