//
//  MatchesCommand.hpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

// Provides matching functionality in a vein similar to POP-11

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class MatchesCommand : public Command
    {
      public:
        MatchesCommand(Function &func_,
                       SharedCacheStack const &sharedCache = SharedCacheStack(),
                       OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~MatchesCommand();

        bool execute() override;
        static std::vector<std::string> getCommandNames();
      private:
        static bool m_registered;
    };

}
