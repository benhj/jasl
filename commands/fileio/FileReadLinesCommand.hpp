//
//  FileReadLinesCommand.hpp
//  jasl
//
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class FileReadLinesCommand : public Command
    {
    public:
        FileReadLinesCommand(Function &func_,
                             SharedCacheStack const &sharedCache = SharedCacheStack(),
                             OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    };

}