//
//  IfCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include <vector>

namespace jasl
{

    class IfCommand : public Command
    {

    public:
        IfCommand(Function &func_, 
                  SharedCacheStack const &sharedCache,
                  OptionalOutputStream const &output = OptionalOutputStream());
        bool execute() override;

    private:
        bool interpretFunctionBody();
        bool parseCommands(std::vector<Function> &functions);
    };

}