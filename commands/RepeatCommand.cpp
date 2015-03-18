//
//  RepeatCommand.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "RepeatCommand.hpp"
#include "../CommandInterpretor.hpp"
#include <string>

namespace jasl {

    RepeatCommand::RepeatCommand(Function &func_, OptionalOutputStream const &output)
    : Command(func_, output)
    {

    }

    bool RepeatCommand::execute() 
    {

        // how many time should repeat loop for?
        auto extracted = VarExtractor::trySingleIntExtraction(m_func.paramA);
        if(!extracted) {
            setLastErrorMessage("repeat: problem extracting integer");
            return false;
        }
        return doLoop(*extracted);
    }

    bool RepeatCommand::doLoop(int const loopCount)
    {
        for (int loop = 0; loop < loopCount; ++loop) {

            // parse commands here
            std::vector<Function> innerFuncs;
            bool success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramB);
            if (success) {
                success = parseCommands(innerFuncs);
            } else {
                setLastErrorMessage("repeat: Error interpreting repeat's body");
                return false;
            }
        }
        return true;
    }

    bool RepeatCommand::parseCommands(std::vector<Function> &functions) 
    {
        CommandInterpretor ci;
        for(auto & f : functions) {
            (void)ci.interpretFunc(f, m_outputStream);
        }
        return true;
    }
}