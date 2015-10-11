//
//  RepeatCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include <string>
#include <vector>

namespace jasl {

    class RepeatCommand : public Command
    {
    public:
        RepeatCommand(Function &func_,
                      SharedVarCache const &sharedCache = SharedVarCache(),
                      OptionalOutputStream const &output = OptionalOutputStream());
        bool execute() override;
    private:
        bool doLoop(int const loopCount);
        bool parseCommands(std::vector<Function> &functions);
    };

}