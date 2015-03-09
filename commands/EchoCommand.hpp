#pragma once

#include "Command.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"

namespace lightlang
{
    class EchoCommand : public Command
    {
    public:
        EchoCommand(Function &func_,
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
            appendToOutput(echoString);
            return true;
        }
    };
}