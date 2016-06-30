//
//  ArrayCommand.hpp
//  jasl
//
//  Created by Ben Jones on 30/06/16
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class ArrayCommand : public Command
    {
    public:
        ArrayCommand(Function &func_,
                     SharedVarCache const &sharedCache = SharedVarCache(),
                     OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:
        bool handleInt();
        bool handleDouble();
        std::string m_type;
        std::string m_varName;

    };

}