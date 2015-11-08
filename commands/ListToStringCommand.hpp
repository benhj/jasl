//
//  ListToStringCommand.hpp
//  jasl
//
//  Created by Ben Jones on 16/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class ListToStringCommand : public Command
    {
    public:
        ListToStringCommand(Function &func_,
                            SharedVarCache const &sharedCache = SharedVarCache(),
                            OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:

        bool tryRawListExtraction(std::string const &varName);
        bool trySymbolExtraction(std::string const &varName);
    };

}