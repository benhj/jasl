//
//  ReturnableCommand.hpp
//  jasl
//
//  Created by Ben Jones on on 10/10/2015.
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"
#include <string>

namespace jasl 
{
    class ReturnableCommand : public Command
    {

        std::string m_functionName;
        std::string m_returnSymbol;
        Type m_returnType;

    public:
        ReturnableCommand(Function &func_,
                      	  SharedCacheStack const &sharedCache = SharedCacheStack(),
                      	  OptionalOutputStream const &output = OptionalOutputStream());
    	bool execute();
       static std::vector<std::string> getCommandNames();
    private:
        bool interpretFunctionBody();
    	bool parseCommands(std::vector<Function> &functions);
        static bool m_registered;
    };
}