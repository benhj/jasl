//
//  IfCommand.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "IfCommand.hpp"
#include "expressions/ComparisonExpression.hpp"
#include "other/CommandInterpretor.hpp"
#include "caching/VarExtractor.hpp"
#include <vector>

namespace {
    // We're in new scope so add a new cache map to the cache stack
    jasl::SharedCacheStack withNewCache(jasl::SharedCacheStack const &sharedCache) {
        auto cloned = sharedCache->clone();
        cloned->pushCacheMap();
        return cloned;
    }
}

namespace jasl
{

    IfCommand::IfCommand(Function &func_,
                         SharedCacheStack const &sharedCache,
                         OptionalOutputStream const &output)
        : Command(func_, withNewCache(sharedCache), output)
    {
    }

    bool IfCommand::execute() 
    {
        return interpretFunctionBody();
    }

    bool IfCommand::interpretFunctionBody()
    {
        bool success = true;

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
            success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramB);
            if (success) {
                success = parseCommands(innerFuncs);
            } else {
                setLastErrorMessage("IfCommand: Error interpreting if statement");
                return false;
            }
        } else {
            std::vector<Function> innerFuncs;
            success = VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramC);
            if (success) {
                success = parseCommands(innerFuncs);
            } else {
                setLastErrorMessage("IfCommand: Error interpreting if statement");
                return false;
            }
        }
        return success;
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