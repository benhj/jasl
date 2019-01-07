//
//  FileReadBytesCommand.hpp
//  jasl
//
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"

namespace jasl
{
    class FileReadBytesCommand : public Command
    {
    public:
        FileReadBytesCommand(Function &func_,
                             SharedCacheStack const &sharedCache = SharedCacheStack(),
                             OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
        static bool m_registered;
    };

}