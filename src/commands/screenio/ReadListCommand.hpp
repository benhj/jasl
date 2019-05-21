//
//  ReadListCommand.hpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"
#include <iostream>

namespace jasl
{
    class ReadListCommand : public Command
    {
      public:
        ReadListCommand(Function &func_,
                        SharedCacheStack const &sharedCache = SharedCacheStack(),
                        OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~ReadListCommand();
        bool execute() override;
        static std::vector<std::string> getCommandNames();

      private:
        bool tryLiteralExtraction();
        bool trySymbolExtraction();
        static bool m_registered;
    };

}
