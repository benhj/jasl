//
//  LengthCommand.hpp
//  jasl
//
//  Created by Ben Jones on 13/03/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class LengthCommand : public Command
    {
    public:
        LengthCommand(Function &func_,
                      SharedVarCache const &sharedCache = SharedVarCache(),
                      OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    private:
        bool tryLiteralExtraction(std::string const &varName);
        bool trySymbolExtraction(std::string const &varName);
    };
}
