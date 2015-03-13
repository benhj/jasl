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
#include "../VarCache.hpp"
#include <sstream>

namespace jasl
{
    class ArgsCommand : public Command
    {
    public:
        ArgsCommand(Function &func_,
                      OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {
        }

        bool execute() override
        {
            int arg;
            if(!m_func.getValueA<int>(arg)) {
                m_errorMessage = "args: couldn't parse argument index";
                appendToOutput(m_errorMessage);
                return false;
            }

            std::string argString;
            if(!m_func.getValueB<std::string>(argString)) {
                m_errorMessage = "args: couldn't parse argument string variable name";
                appendToOutput(m_errorMessage);
                return false;
            }

            if(arg < VarCache::args.size()) {
                auto argument(VarCache::args[arg]);
                VarCache::stringCache[argString] = argument;
                return true;
            }

            return false;
        }
    };

}