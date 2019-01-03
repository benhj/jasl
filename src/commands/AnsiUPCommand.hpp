//
//  AnsiUPCommand.hpp
//  jasl
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    /// For moving cursor up by N spaces
    /// ansi_up 5;
    class AnsiUPCommand : public Command
    {
    public:
        AnsiUPCommand(Function &func_,
                      SharedCacheStack const &sharedCache = SharedCacheStack(),
                      OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    };

}
