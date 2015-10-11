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
#include <iostream>

namespace jasl
{
    class InputCommand : public Command
    {
    public:
        InputCommand(Function &func_,
                     SharedVarCache const &sharedCache = SharedVarCache(),
                     OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {

        }

        bool execute() override
        {
            if(!tryLiteralExtraction()) {
                if(!trySymbolExtraction()) { return false; }
            }

            std::string answer;
            if(!m_func.getValueB<std::string>(answer, m_sharedCache)) {
                setLastErrorMessage("input: couldn't parse answer variable");
                return false;
            }

            std::string result;
            std::getline(std::cin, result);
            m_sharedCache->setString(answer, result);
            return false;
        }

    private:
        bool tryLiteralExtraction() 
        {
            LiteralString query;
            if(m_func.getValueA<LiteralString>(query, m_sharedCache)) {
                appendToOutput(query.literal);
                return true;
            }
            return false;
        }

        bool trySymbolExtraction()
        {
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
                {
                    auto result = m_sharedCache->getString(symbol);
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