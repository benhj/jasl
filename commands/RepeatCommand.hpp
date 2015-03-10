//
//  RepeatCommand.hpp
//  lightlang
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include <string>
#include <vector>

namespace lightlang {

    class RepeatCommand : public Command
    {
    public:
        RepeatCommand(Function &func_, OptionalOutputStream const &output);
        bool execute() override;
    private:
        bool doLoop(int const loopCount);
        bool parseCommands(std::vector<Function> &functions);
    };

}