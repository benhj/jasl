//
//  StringToPrimitiveCommand.hpp
//  jasl
//
//  Created by Ben Jones on 13/03/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class StringToPrimitiveCommand : public Command
    {
    public:
        StringToPrimitiveCommand(Function &func_,
                                 SharedCacheStack const &sharedCache = SharedCacheStack(),
                                 OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
    private:
        bool tryLiteralExtraction(std::string const &varName);
        bool trySymbolExtraction(std::string const &varName);
    };
}
