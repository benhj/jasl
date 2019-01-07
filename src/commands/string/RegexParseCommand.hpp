//
//  RegexParseCommand.hpp
//  jasl  
//  
//  Copyright (c) 2018 Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class RegexParseCommand : public Command
    {  
      public:
        RegexParseCommand(Function &func_,
                       	  SharedCacheStack const &sharedCache = SharedCacheStack(),
                       	  OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
        static std::vector<std::string> getCommandNames();
      private:
        static bool m_registered;
    };

}
