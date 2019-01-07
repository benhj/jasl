//
//  ArrayPutCommand.hpp
//  jasl
//
//  Created by Ben Jones on 30/06/15
//  Copyright (c) 2016-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"

namespace jasl
{
    class ArrayPutCommand : public Command
    {
    public:
        ArrayPutCommand(Function &func_,
                        SharedCacheStack const &sharedCache = SharedCacheStack(),
                        OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    private:
        bool getIndex(int64_t &index);
        std::string m_type;
        std::string m_varName;
        static bool m_registered;
    };
}