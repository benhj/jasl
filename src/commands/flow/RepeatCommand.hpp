//
//  RepeatCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"
#include <string>
#include <vector>
#include <memory>

namespace jasl {

    class RepeatCommand : public Command
    {
    public:
        RepeatCommand(Function &func_,
                      SharedCacheStack const &sharedCache = SharedCacheStack(),
                      OptionalOutputStream const &output = OptionalOutputStream());
        bool execute() override;
    private:
        bool doLoop();
        bool parseCommands();
        bool m_weHaveFuncs;
        int64_t m_loopCount;
        std::vector<std::shared_ptr<Command>> m_commands;
        static bool m_registered;
    };

}