//
//  InputCommand.cpp
//  jasl
//
//  Created by Ben Jones on 14/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../LiteralString.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"
#include <iostream>

namespace jasl
{
    class InputCommand : public Command
    {
    public:
        InputCommand(Function &func_,
                    OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {

        }

        bool execute() override
        {
            if(!tryLiteralExtraction()) {
                if(!trySymbolExtraction()) { return false; }
            }

            std::string answer;
            if(!m_func.getValueB<std::string>(answer)) {
                setLastErrorMessage("input: couldn't parse answer variable");
                return false;
            }

            std::getline(std::cin, VarCache::stringCache[answer]);
            return false;
        }

    private:
        bool tryLiteralExtraction() 
        {
            LiteralString query;
            if(m_func.getValueA<LiteralString>(query)) {
                appendToOutput(query.literal);
                return true;
            }
            return false;
        }

        bool trySymbolExtraction()
        {
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol)) {
                {
                    auto result = VarCache::getString(symbol);
                    if(result) {
                        appendToOutput(*result);
                        return true;
                    }
                }
                return false;
            }
            return false;
        }

    };

}