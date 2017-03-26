//
//  InputCommand.hpp
//  jasl
//
//  Created by Ben Jones on 14/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include <iostream>

namespace jasl
{
    class InputCommand : public Command
    {
    public:
        InputCommand(Function &func_,
                     SharedCacheStack const &sharedCache = SharedCacheStack(),
                     OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:
        bool tryLiteralExtraction();
        bool trySymbolExtraction();

    };

}