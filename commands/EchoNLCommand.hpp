//
//  EchoNLCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"

namespace jasl
{
    class EchoNLCommand : public Command
    {
    public:
        EchoNLCommand(Function &func_,
                      OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {

        }

        bool execute() override
        {
            std::string echoString;
            if(!m_func.getValueA<std::string>(echoString)) {
                m_errorMessage = "echo: problem getting string";
                appendToOutput(m_errorMessage);
                return false;
            }
            appendToOutputWithNewLine(echoString);
            return true;
        }
    };
}