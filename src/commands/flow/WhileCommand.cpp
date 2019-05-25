//
//  WhileCommand.cpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "WhileCommand.hpp"
#include "caching/WithNewCache.hpp"
#include "core/CommandInterpretor.hpp"
#include "core/ExceptionDispatcher.hpp"
#include "core/BreakControlFlowException.hpp"
#include "core/RegisterCommand.hpp"
#include <string>

bool jasl::WhileCommand::m_registered = 
registerCommand<jasl::WhileCommand>();

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

    WhileCommand::~WhileCommand() = default;

    std::vector<std::string> WhileCommand::getCommandNames()
    {
        return {"while"};
    }

    bool WhileCommand::execute() 
    {
        if(!m_weHaveFuncs) {
            return false;
        }
        auto goodtogo(VarExtractor::trySingleBoolExtraction_V2(m_func.paramA, m_sharedCache));
        try {
            while(goodtogo()) {
                if (!parseCommands()) {
                    setLastErrorMessage("repeat: Error interpreting while's body");
                    return false;
                }
            }
        } catch (BreakControlFlowException const &) {
            // Corresponds to a 'break'
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
                handleException();
            }
        }
        return true;
    }
}
