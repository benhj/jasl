//
//  ExecCommand.cpp
//  jasl
//
//  Created by Ben Jones on 21/09/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../VarExtractor.hpp"
#include <sstream>

namespace jasl
{
    class ExecCommand : public Command
    {
    public:
        ExecCommand(Function &func_,
                    SharedVarCache const &sharedCache = SharedVarCache(),
                    OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {

        }

        bool execute() override
        {
            if(tryLiteralExtraction()) { return true; }
            if(trySymbolExtraction()) { return true; }
            setLastErrorMessage("exec: couldn't parse");
            return false;
        }

    private:

        bool tryLiteralExtraction() 
        {
            LiteralString literalString;
            if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
                system(literalString.literal.c_str());
                return true;
            }
            return false;
        }

        bool trySymbolExtraction()
        {
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
            	auto result = m_sharedCache->getString(symbol);
                if(result) {
                    system((*result).c_str());
                    return true;
                }
            }
            return false;
        }
    };

}