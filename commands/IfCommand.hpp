#pragma once

#include "Command.hpp"
#include <vector>

namespace lightlang
{

    class IfCommand : public Command
    {

    public:
        IfCommand(Function &func_, OptionalOutputStream const &output);
        bool execute() override;

    private:
        bool interpretFunctionBody();
        bool parseCommands(std::vector<Function> &functions);
    };

}