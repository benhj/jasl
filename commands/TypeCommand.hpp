//
//  TypeCommand.hpp
//  jasl
//
//  Created by Ben Jones on 03/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class TypeCommand : public Command
    {
    public:
        TypeCommand(Function &func_,
                    SharedVarCache const &sharedCache = SharedVarCache(),
                    OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    };

}
