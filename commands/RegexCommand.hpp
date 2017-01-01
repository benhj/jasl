//
//  RegexCommand.hpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class RegexCommand : public Command
    {
    public:
        RegexCommand(Function &func_,
                     SharedCacheStack const &sharedCache = SharedCacheStack(),
                     OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    };

}
