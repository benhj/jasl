//
//  RepeatCommand.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "RepeatCommand.hpp"
#include "caching/WithNewCache.hpp"
#include "core/CommandInterpretor.hpp"
#include "core/RegisterCommand.hpp"
#include <string>

namespace jasl {

    RepeatCommand::RepeatCommand(Function &func_,
                                 SharedCacheStack const &sharedCache,
                                 OptionalOutputStream const &output)
        : Command(func_, withNewCache(sharedCache), output)
        , m_weHaveFuncs(false)
        , m_commands()
        , m_loopCount(0)
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
            if(!VarExtractor::trySingleIntExtraction(m_func.paramA, m_loopCount, m_sharedCache)) {
                setLastErrorMessage("repeat: problem extracting int");
                m_weHaveFuncs = false;
            }
        }
    }

    std::vector<std::string>RepeatCommand::getCommandNames()
    {
        return {"repeat"};
    }

    bool RepeatCommand::execute() 
    {
        if(!m_weHaveFuncs) {
            return false;
        }
        return doLoop();
    }

    bool RepeatCommand::doLoop()
    {
        for (int64_t loop = 0; loop < m_loopCount; ++loop) {
            // parse commands here
            if (!parseCommands()) {
                setLastErrorMessage("repeat: Error interpreting repeat's body");
                return false;
            }
        }
        return true;
    }

    bool RepeatCommand::parseCommands() 
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