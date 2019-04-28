//
//  TieCommand.hpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class TieCommand : public Command
    {
      public:
        TieCommand(Function &func_,
                    SharedCacheStack const &sharedCache = SharedCacheStack(),
                    OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~TieCommand();

        bool execute() override;
        static std::vector<std::string> getCommandNames();
      private:
        static bool m_registered;
    };

}
