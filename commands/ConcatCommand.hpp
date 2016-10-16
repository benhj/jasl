//
//  ConcatCommand.hpp
//  jasl
//
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class ConcatCommand : public Command
    {
    public:
        ConcatCommand(Function &func_,
                      SharedVarCache const &sharedCache = SharedVarCache(),
                      OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:
        bool tryConcatExtraction(std::string const & key);
    };

}
