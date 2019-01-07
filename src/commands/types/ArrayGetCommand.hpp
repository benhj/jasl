//
//  ArrayGetCommand.hpp
//  jasl
//
//  Created by Ben Jones on 30/06/15
//  Copyright (c) 2016-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"
#include <string>

namespace jasl
{
    class ArrayGetCommand : public Command
    {
      public:
        ArrayGetCommand(Function &func_,
                        SharedCacheStack const &sharedCache = SharedCacheStack(),
                        OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
        static std::vector<std::string> getCommandNames();
      private:
        bool getIndex(int64_t&);
        std::string m_arrayName;
        std::string m_varName;
        std::string m_type;
        static bool m_registered;

    };

}