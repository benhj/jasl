//
//  WhileCommand.cpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "WhileCommand.hpp"
#include "../CommandInterpretor.hpp"
#include <string>

namespace {
    // We're in new scope so add a new cache map to the cache stack
    jasl::SharedCacheStack withNewCache(jasl::SharedCacheStack const &sharedCache) {
        auto cloned = sharedCache->clone();
        cloned->pushCacheMap();
        return cloned;
    }
}

namespace jasl {

    WhileCommand::WhileCommand(Function &func_,
                               SharedCacheStack const &sharedCache,
                               OptionalOutputStream const &output)
    : Command(func_, withNewCache(sharedCache), output)
    , m_weHaveFuncs(false)
    , m_commands()
    {
        std::vector<Function> innerFuncs;
        if(VarExtractor::tryAnyCast<std::vector<Function>>(innerFuncs, m_func.paramB)) {
            m_weHaveFuncs = true;
        }
        if(m_weHaveFuncs) {
            CommandInterpretor ci;
            for(auto & f : innerFuncs) {
                m_commands.push_back(ci.funcToCommand(f, sharedCache, output));
            }
        }
    }

    bool WhileCommand::execute() 
    {
        if(!m_weHaveFuncs) {
            return false;
        }
        auto goodtogo(VarExtractor::trySingleBoolExtraction_V2(m_func.paramA, m_sharedCache));
        while(goodtogo()) {
            if (!parseCommands()) {
                setLastErrorMessage("repeat: Error interpreting while's body");
                return false;
            }
        }
        return true;
    }

    bool WhileCommand::parseCommands() 
    {
        for(auto & c : m_commands) {
            try {
                if(c) {
                    c->execute(); 
                }
            } catch (...) {

            }
        }
        return true;
    }
}