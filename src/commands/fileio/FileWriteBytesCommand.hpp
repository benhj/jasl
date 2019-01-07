//
//  FileWriteBytesCommand.hpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"

namespace jasl
{
    class FileWriteBytesCommand : public Command
    {
      public:
        FileWriteBytesCommand(Function &func_,
                              SharedCacheStack const &sharedCache = SharedCacheStack(),
                              OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
        static std::vector<std::string> getCommandNames();
      private:
        static bool m_registered;
    };

}