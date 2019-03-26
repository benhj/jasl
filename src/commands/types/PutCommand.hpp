//
//  PutCommand.hpp
//  jasl
//
//  Created by Ben Jones on 06/07/16
//  Copyright (c) 2016-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"

namespace jasl
{
    class PutCommand : public Command
    {
      public:
        PutCommand(Function &func_,
                   SharedCacheStack const &sharedCache = SharedCacheStack(),
                   OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
        static std::vector<std::string> getCommandNames();

        virtual ~PutCommand();

      private:
        bool m_primitive;
        static bool m_registered;
    };

}
