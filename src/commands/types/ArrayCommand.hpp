//
//  ArrayCommand.hpp
//  jasl
//
//  Created by Ben Jones on 30/06/16
//  Copyright (c) 2016-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"

namespace jasl
{
    class ArrayCommand : public Command
    {
    public:
        ArrayCommand(Function &func_,
                     SharedCacheStack const &sharedCache = SharedCacheStack(),
                     OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:
        bool handleInt();
        bool handleReal();
        std::string m_type;
        std::string m_varName;

    };

}