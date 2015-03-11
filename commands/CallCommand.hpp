//
//  CallCommand.hpp
//  jasl
//
//  Created by Ben Jones on on 10/03/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl 
{
    class CallCommand : public Command
    {
    public:
        CallCommand(Function &func_, OptionalOutputStream const &output);
        bool execute() override;
    private:
        bool searchForFunction(std::string &functionName);
        bool parseCommand(Function &function);
    };
}