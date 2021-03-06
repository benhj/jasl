//
//  ListTokenIndexCommand.hpp
//  jasl
//
//  Created by Ben Jones on 16/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class ListTokenIndexCommand : public Command
    {
      public:
        
        ListTokenIndexCommand(Function &func_,
                              SharedCacheStack const &sharedCache = SharedCacheStack(),
                              OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~ListTokenIndexCommand();

        bool execute() override;
        static std::vector<std::string> getCommandNames();

      private:

        OptionalString getTestToken();
        bool tryWithRawList(std::string const &varName);
        bool tryWithSymbolList(std::string const &varName);
        static bool m_registered;
    };

}
