//
//  IfCommand.hpp
//  lightlang
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

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