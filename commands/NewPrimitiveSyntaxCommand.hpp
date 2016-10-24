//
//  NewPrimitiveSyntaxCommand.hpp
//  jasl
//
//  Created by Ben Jones on 13/03/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl {
    class NewPrimitiveSyntaxCommand : public Command
    {
    public:
        NewPrimitiveSyntaxCommand(Function &func_,
                                  SharedCacheStack const &sharedCache = SharedCacheStack(),
                                  OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:
        bool handleInt();
        bool handleReal();
        bool handleBool();
        bool handleByte();

        std::string m_type;
        std::string m_varName;
    };
}