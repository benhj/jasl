//
//  JoinAllCommand.hpp
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
    class JoinAllCommand : public Command
    {
    public:
        JoinAllCommand(Function &func_,
                       SharedCacheStack const &sharedCache = SharedCacheStack(),
                       OptionalOutputStream const &output = OptionalOutputStream());
        bool execute() override;
        static std::vector<std::string> getCommandNames();
    private:
        static bool m_registered;
    };
}