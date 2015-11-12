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
#include <memory>

namespace jasl {

    class WhileCommand : public Command
    {
    public:
        WhileCommand(Function &func_,
                     SharedVarCache const &sharedCache = SharedVarCache(),
                     OptionalOutputStream const &output = OptionalOutputStream());
        bool execute() override;
    private:
        bool parseCommands();
        bool m_weHaveFuncs;
        std::vector<std::shared_ptr<Command>> m_commands;
    };

}