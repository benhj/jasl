//
//  EchoNLCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class EchoNLCommand : public Command
    {
    public:
        EchoNLCommand(Function &func_,
                      SharedVarCache const &sharedCache = SharedVarCache(),
                      OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:
        bool tryLiteralExtraction();
        bool trySymbolExtraction();
        void processListElement(ValueArray const &valueArray, std::string &output);
        bool tryNumericExtraction();
    };
}