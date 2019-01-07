//
//  ExecCommand.hpp
//  jasl
//
//  Created by Ben Jones on 21/09/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class ExecCommand : public Command
    {
    public:
        ExecCommand(Function &func_,
                    SharedCacheStack const &sharedCache = SharedCacheStack(),
                    OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:
        bool tryLiteralExtraction();
        bool trySymbolExtraction();
        static bool m_registered;
    };

}