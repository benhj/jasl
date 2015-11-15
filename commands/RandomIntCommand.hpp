//
//  RandomIntCommand.hpp
//  jasl
//
//  Created by Ben Jones on 05/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl {
    class RandomIntCommand : public Command
    {
    public:
        RandomIntCommand(Function &func_,
                         SharedVarCache const &sharedCache = SharedVarCache(),
                         OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    };
}
