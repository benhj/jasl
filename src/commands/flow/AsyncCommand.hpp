//
//  AsyncCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"
#include <string>
#include <vector>
#include <memory>

namespace jasl {
    class AsyncCommand : public Command
    {
    public:
        AsyncCommand(Function &func_,
                     SharedCacheStack const &sharedCache = SharedCacheStack(),
                     OptionalOutputStream const &output = OptionalOutputStream());
        bool execute() override;
    private:
        bool parseCommandsAsync();
        bool m_weHaveFuncs;
        std::vector<std::shared_ptr<Command>> m_commands;
        static bool m_registered;
    };
}