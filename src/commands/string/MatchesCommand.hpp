//
//  MatchesCommand.hpp
//  jasl  
//  
//  Copyright (c) 2017-present Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class MatchesCommand : public Command
    {  
    public:
        MatchesCommand(Function &func_,
                       SharedCacheStack const &sharedCache = SharedCacheStack(),
                       OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    };

}
