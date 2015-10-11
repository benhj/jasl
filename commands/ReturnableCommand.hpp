//
//  ReturnableCommand.hpp
//  jasl
//
//  Created by Ben Jones on on 10/10/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl 
{
    class ReturnableCommand : public Command
    {
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