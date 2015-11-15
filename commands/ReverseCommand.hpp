//
//  ReverseCommand.hpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class ReverseCommand : public Command
    {
    public:
        ReverseCommand(Function &func_,
                       SharedVarCache const &sharedCache = SharedVarCache(),
                       OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    };

}
