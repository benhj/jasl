//
//  OneOfCommand.hpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class OneOfCommand : public Command
    {
      public:
        OneOfCommand(Function &func_,
                     SharedCacheStack const &sharedCache = SharedCacheStack(),
                     OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~OneOfCommand();

        bool execute() override;
        static std::vector<std::string> getCommandNames();
      private:
        static bool m_registered;

    };

}
