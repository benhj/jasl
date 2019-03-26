//
//  IfCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"
#include <vector>

namespace jasl
{

    class IfCommand : public Command
    {

    public:
        IfCommand(Function &func_, 
                  SharedCacheStack const &sharedCache,
                  OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~IfCommand();
        bool execute() override;
        static std::vector<std::string> getCommandNames();

    private:
        bool interpretFunctionBody();
        bool parseCommands(std::vector<Function> &functions);
        static bool m_registered;
    };

}
