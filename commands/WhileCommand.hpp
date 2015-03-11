//
//  WhileCommand.hpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include <string>
#include <vector>

namespace jasl {

    class WhileCommand : public Command
    {
    public:
        WhileCommand(Function &func_, OptionalOutputStream const &output);
        bool execute() override;
    private:
        bool doLoop(bool const truth);
        bool parseCommands(std::vector<Function> &functions);
    };

}