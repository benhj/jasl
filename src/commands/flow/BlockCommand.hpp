//
//  BlockCommand.hpp
//  jasl
//
//  Created by Ben Jones on 10/03/2015.
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"
#include <vector>

namespace jasl {
    class BlockCommand : public Command
    {
    public:
        BlockCommand(Function &func_, 
                     SharedCacheStack const &sharedCache = SharedCacheStack(),
                     OptionalOutputStream const &output = OptionalOutputStream());
        bool execute() override;
    private:
        bool interpretFunctionBody();
        bool parseCommands(std::vector<Function> &functions);
        static bool m_registered;
    };
}
