//
//  WildcardParseCommand.hpp
//  jasl  
//  
//  Copyright (c) 2018 Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class WildcardParseCommand : public Command
    {  
    public:
        WildcardParseCommand(Function &func_,
                       	     SharedCacheStack const &sharedCache = SharedCacheStack(),
                       	     OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    };

}
