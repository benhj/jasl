//
//  ReturnableCommand.hpp
//  jasl
//
//  Created by Ben Jones on on 10/10/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "ReturnableCommand.hpp"
#include "../CommandInterpretor.hpp"
#include "../GlobalCache.hpp"
#include <vector>

namespace jasl 
{
    ReturnableCommand::ReturnableCommand(Function &func_,
                                         SharedVarCache const &sharedCache,
                                         OptionalOutputStream const &output)
    : Command(func_, std::make_shared<ScopedVarCache>(), output)
    {
    }

    bool ReturnableCommand::execute()
    {
        std::string type;
        (void)m_func.getValueA<std::string>(type, m_sharedCache);
        std::string returnName;
        (void)m_func.getValueD<std::string>(returnName, m_sharedCache);
        std::string functionName; 
        (void)m_func.getValueB<std::string>(functionName, m_sharedCache);

        if(type == "integer") {
            GlobalCache::setInt(returnName, 0);
        } else if(type == "decimal") {
            GlobalCache::setDouble(returnName, 0);
        } else if(type == "string") {
            GlobalCache::setString(returnName, "");
        } else if(type == "boolean") {
            GlobalCache::setBool(returnName, false);
        } else if(type == "list") {
            GlobalCache::setList(returnName, ValueArray());
        } else {
            setLastErrorMessage("returnable: unknown return type");
            return false;
        }

        interpretFunctionBody();
        return true;
    }

    bool ReturnableCommand::interpretFunctionBody()
    {
        std::vector<Function> innerFuncs;
        bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramE);
        if (success) {
            success = parseCommands(innerFuncs);
        } else {
            setLastErrorMessage("returnable: Error interpreting returnable's body");
            return false;
        }
        return true;
    }

    bool ReturnableCommand::parseCommands(std::vector<Function> &functions) 
    {
        CommandInterpretor ci;
        for(auto & f : functions) {
            (void)ci.interpretFunc(f, m_sharedCache, m_outputStream);
        }
        return true;
    }
}