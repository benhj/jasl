//
//  ReleaseCommand.cpp
//  jasl
//
//  Created by Ben Jones on 01/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include <sstream>

namespace jasl
{
    class ReleaseCommand : public Command
    {
    public:
        ReleaseCommand(Function &func_,
                       SharedVarCache const &sharedCache = SharedVarCache(),
                       OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, std::move(sharedCache), std::move(output))
        {
        }

        bool execute() override
        {
            if(trySymbolExtraction()) { return true; }
            setLastErrorMessage("exec: couldn't parse");
            return false;
        }

    private:

        bool trySymbolExtraction()
        {
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
            	m_sharedCache->eraseValue(symbol);
                return true;
            }
            return false;
        }
    };

}