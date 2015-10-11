//
//  BlockCommand.cpp
//  jasl
//
//  Created by Ben Jones on on 10/03/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "BlockCommand.hpp"
#include "../CommandInterpretor.hpp"
#include "../VarExtractor.hpp"


namespace jasl {

    BlockCommand::BlockCommand(Function &func_, 
                               SharedVarCache const &sharedCache, 
                               OptionalOutputStream const &output)
    : Command(func_, SharedVarCache(), output)
    {

    }

    bool BlockCommand::execute()
    {
        return interpretFunctionBody();
    }


    bool BlockCommand::interpretFunctionBody()
    {
        std::vector<Function> innerFuncs;
        bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramC);
        if (success) {
            success = parseCommands(innerFuncs);
        } else {
            setLastErrorMessage("block: Error interpreting block's body");
            return false;
        }
        return true;
    }

    bool BlockCommand::parseCommands(std::vector<Function> &functions) 
    {
        CommandInterpretor ci;
        for(auto & f : functions) {
            (void)ci.interpretFunc(f, m_outputStream);
        }
        return true;
    }
}
