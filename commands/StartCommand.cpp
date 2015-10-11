//
//  StartCommand.cpp
//  jasl
//
//  Created by Ben Jones on on 10/03/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "StartCommand.hpp"
#include "../CommandInterpretor.hpp"
#include "../VarExtractor.hpp"


namespace jasl {

    StartCommand::StartCommand(Function &func_,
                               SharedVarCache const &sharedCache,
                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output) 
    {
    }

    bool StartCommand::execute()
    {
        return interpretFunctionBody();
    }


    bool StartCommand::interpretFunctionBody()
    {
        std::vector<Function> innerFuncs;
        bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramA);
        if (success) {
            success = parseCommands(innerFuncs);
        } else {
            setLastErrorMessage("start: Error interpreting start's body");
            return false;
        }
        return true;
    }

    bool StartCommand::parseCommands(std::vector<Function> &functions) 
    {
        CommandInterpretor ci;
        for(auto & f : functions) {
            (void)ci.interpretFunc(f, m_outputStream);
        }
        return true;
    }
}
