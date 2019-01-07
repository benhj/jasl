//
//  EchoNLCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"

namespace jasl
{
    class EchoNLCommand : public Command
    {
      public:
        EchoNLCommand(Function &func_,
                      SharedCacheStack const &sharedCache = SharedCacheStack(),
                      OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
        static std::vector<std::string> getCommandNames();

      private:
        bool tryLiteralExtraction();
        bool trySymbolExtraction();
        void processListElement(List const &valueArray, std::string &output);
        bool tryNumericExtraction();
        static bool m_registered;
    };
}