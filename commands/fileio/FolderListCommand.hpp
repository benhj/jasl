//
//  FolderListCommand.hpp
//  jasl
//
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class FolderListCommand : public Command
    {
    public:
        FolderListCommand(Function &func_,
                          SharedCacheStack const &sharedCache = SharedCacheStack(),
                          OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    };

}