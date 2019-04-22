//
//  EchoCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"

namespace jasl
{
    class EchoCommand : public Command
    {
      public:
        EchoCommand(Function &func_,
                    SharedCacheStack const &sharedCache = SharedCacheStack(),
                    OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~EchoCommand();
        bool execute() override;
        static std::vector<std::string> getCommandNames();

      private:
        bool tryLiteralExtraction();
        bool trySymbolExtraction();
        void processListElement(List const &valueArray,
                                std::string &output,
                                int const depth = 0,
                                bool const lastToken = false);
        bool tryNumericExtraction();
        static bool m_registered;
    };
}
