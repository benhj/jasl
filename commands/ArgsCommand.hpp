//
//  ArgsCommand.cpp
//  jasl
//
//  Created by Ben Jones on 13/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../VarExtractor.hpp"
#include "../GlobalCache.hpp"
#include <sstream>
#include <cstdint>

namespace jasl
{
    class ArgsCommand : public Command
    {
    public:
        ArgsCommand(Function &func_,
                    SharedVarCache const &sharedCache = SharedVarCache(),
                    OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {
        }

        bool execute() override
        {

            auto arg = VarExtractor::trySingleIntExtraction(m_func.paramA, m_sharedCache);
            if(!arg) {
                setLastErrorMessage("repeat: problem extracting integer");
                return false;
            }

            std::string argString;
            if(!m_func.getValueB<std::string>(argString, m_sharedCache)) {
                setLastErrorMessage("args: couldn't parse argument string variable name");
                return false;
            }

            if(*arg < GlobalCache::args.size()) {
                auto argument(GlobalCache::args[*arg]);
                m_sharedCache->setString(argString, argument);
                return true;
            }

            return false;
        }
    };

}