//
//  AppendCommand.hpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class AppendCommand : public Command
    {
      public:
        AppendCommand(Function &func_,
                      SharedCacheStack const &sharedCache = SharedCacheStack(),
                      OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
        static std::vector<std::string> getCommandNames();

      private:

        OptionalString getStringBeingAppendedTo();
        bool tryLiteralExtraction(std::string const &key);
        bool trySymbolExtraction(std::string const &key);
        bool tryNumericExtraction(std::string const &key);
        static bool m_registered;

    };

}