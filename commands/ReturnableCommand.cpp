//
//  ReturnableCommand.hpp
//  jasl
//
//  Created by Ben Jones on on 10/10/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "ReturnableCommand.hpp"
#include "../CommandInterpretor.hpp"
#include <vector>

namespace jasl 
{
    ReturnableCommand::ReturnableCommand(Function &func_,
                                         SharedVarCache const &sharedCache,
                                         OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool ReturnableCommand::execute()
    {
        std::string type;
        (void)m_func.getValueA<std::string>(type);
        std::string returnName;
        (void)m_func.getValueD<std::string>(returnName);
        std::string functionName; 
        (void)m_func.getValueB<std::string>(functionName);

        if(type == "integer") {
            VarCache::setInt(returnName, 0);
        } else if(type == "decimal") {
            VarCache::setDouble(returnName, 0);
        } else if(type == "string") {
            VarCache::setString(returnName, "");
        } else if(type == "boolean") {
            VarCache::setBool(returnName, false);
        } else if(type == "list") {
            VarCache::setList(returnName, ValueArray());
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
            (void)ci.interpretFunc(f, m_outputStream);
        }
        return true;
    }
}