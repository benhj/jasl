//
//  EchoCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class EchoCommand : public Command
    {
    public:
        EchoCommand(Function &func_,
                    SharedCacheStack const &sharedCache = SharedCacheStack(),
                    OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:
        bool tryLiteralExtraction();
        bool trySymbolExtraction();
        void processListElement(List const &valueArray,
                                std::string &output);
        bool tryNumericExtraction();
    };
}