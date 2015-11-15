//
//  ReleaseCommand.hpp
//  jasl
//
//  Created by Ben Jones on 01/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class ReleaseCommand : public Command
    {
    public:
        ReleaseCommand(Function &func_,
                       SharedVarCache const &sharedCache = SharedVarCache(),
                       OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:
        bool trySymbolExtraction();
    };

}
