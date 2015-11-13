//
//  ReturnableCommand.hpp
//  jasl
//
//  Created by Ben Jones on on 10/10/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
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
                      	  SharedVarCache const &sharedCache = SharedVarCache(),
                      	  OptionalOutputStream const &output = OptionalOutputStream());
    	bool execute();
    private:
        bool interpretFunctionBody();
    	bool parseCommands(std::vector<Function> &functions); 
    };
}