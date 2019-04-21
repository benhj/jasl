//
//  FindCommand.hpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

// Provides matching functionality in a vein similar to POP-11's present command.

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class FindCommand : public Command
    {
      public:
        FindCommand(Function &func_,
                    SharedCacheStack const &sharedCache = SharedCacheStack(),
                    OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~FindCommand();

        bool execute() override;
        static std::vector<std::string> getCommandNames();
      private:
        static bool m_registered;
    };

}
