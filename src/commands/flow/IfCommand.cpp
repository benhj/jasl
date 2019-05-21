//
//  IfCommand.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "IfCommand.hpp"
#include "commands/expressions/ComparisonExpression.hpp"
#include "core/CommandInterpretor.hpp"
#include "core/RegisterCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "caching/WithNewCache.hpp"
#include <vector>

/**
 * AUTO TYPE REGISTRAION
 */
bool jasl::IfCommand::m_registered = registerCommand<jasl::IfCommand>();

namespace jasl
{

    IfCommand::IfCommand(Function &func_,
                         SharedCacheStack const &sharedCache,
                         OptionalOutputStream const &output)
        : Command(func_, withNewCache(sharedCache), output)
    {
    }

    IfCommand::~IfCommand() = default;

    std::vector<std::string> IfCommand::getCommandNames()
    {
        return {"if","?"};
    }

    bool IfCommand::execute() 
    {
        return interpretFunctionBody();
    }

    bool IfCommand::interpretFunctionBody()
    {
        // Extract the bool condition of the if-statement
        ComparisonExpression ce;
        if (!VarExtractor::tryExtraction<ComparisonExpression>(ce, m_func.paramA, m_sharedCache)) {
            return false;
        }
        ce.m_sharedCache = m_sharedCache;

        // Basically do the if statement and then parse the functions
        // within the if block
        if (ce.evaluate()) {
            std::vector<Function> innerFuncs;
            auto const success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramB);
            if (success) {
                return parseCommands(innerFuncs);
            } else {
                setLastErrorMessage("IfCommand: Error interpreting if statement");
                return false;
            }
        } else if(m_func.name != "elseif") {

            // See if we have a bunch of elseif parts
            std::vector<Function> innerFuncs;
            auto success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramC);
            if (success && !innerFuncs.empty()) {
                // If the name of the first function parsed out of
                // paramC isn't 'elseif', we must be in the else branch
                // and so can proceed with parsing 'else's' commands.
                if(innerFuncs[0].name != "elseif") {
                    return parseCommands(innerFuncs);
                } else {
                    // Else the parts are ifelse branches, so we can recurse into
                    // these by constructing additional if commands and returning
                    // true on the first one that is successful.
                    for(auto & ei : innerFuncs) {
                        if(IfCommand(ei, m_sharedCache, m_outputStream).execute()) {
                            return true;
                        }
                    }
                    // Dind't return true so see if we have a final 'else' bit
                    std::vector<Function> elseFuncs;
                    success = VarExtractor::tryAnyCast<std::vector<Function>>(elseFuncs, m_func.paramD);
                    if(success) {
                        return parseCommands(elseFuncs);
                    }
                }
            }
            else {
                setLastErrorMessage("IfCommand: Error interpreting if statement");
                return false;
            }
        }
        return false;
    }

    bool IfCommand::parseCommands(std::vector<Function> &functions) 
    {
        CommandInterpretor ci;
        for(auto & f : functions) {
            (void)ci.interpretFunc(f, m_sharedCache, m_outputStream);
        }
        return true;
    }
}
