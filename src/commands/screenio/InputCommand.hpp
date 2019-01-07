//
//  InputCommand.hpp
//  jasl
//
//  Created by Ben Jones on 14/03/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"
#include <iostream>

namespace jasl
{
    class InputCommand : public Command
    {
      public:
        InputCommand(Function &func_,
                     SharedCacheStack const &sharedCache = SharedCacheStack(),
                     OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
        static std::vector<std::string> getCommandNames();

      private:
        bool tryLiteralExtraction();
        bool trySymbolExtraction();
        static bool m_registered;
    };

}