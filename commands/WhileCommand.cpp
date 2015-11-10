//
//  WhileCommand.cpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "WhileCommand.hpp"
#include "../CommandInterpretor.hpp"
#include <string>

namespace jasl {

    WhileCommand::WhileCommand(Function &func_,
                               SharedVarCache const &sharedCache,
                               OptionalOutputStream const &output)
    : Command(func_, std::move(sharedCache), std::move(output))
    {
    }

    bool WhileCommand::execute() 
    {
        return doLoop();
    }

    bool WhileCommand::doLoop()
    {

        std::vector<Function> innerFuncs;
        bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramB);
        if(!success) {
            return false;
        }
        auto goodtogo = VarExtractor::trySingleBoolExtraction_V2(m_func.paramA, m_sharedCache);

        while(goodtogo()) {
            if (success) {
                success = parseCommands(innerFuncs);
            } else {
                setLastErrorMessage("repeat: Error interpreting while's body");
                return false;
            }
        }

        return true;
    }

    bool WhileCommand::parseCommands(std::vector<Function> &functions) 
    {
        CommandInterpretor ci;
        for(auto & f : functions) {
            (void)ci.interpretFunc(f, m_sharedCache, m_outputStream);
        }
        return true;
    }
}