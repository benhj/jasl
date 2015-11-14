//
//  InputCommand.cpp
//  jasl
//
//  Created by Ben Jones on 8/11/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "InputCommand.hpp"
#include "../LiteralString.hpp"
#include "../VarExtractor.hpp"

namespace jasl
{
    InputCommand::InputCommand(Function &func_,
                               SharedVarCache const &sharedCache,
                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool InputCommand::execute()
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

    bool InputCommand::tryLiteralExtraction() 
    {
        LiteralString query;
        if(m_func.getValueA<LiteralString>(query, m_sharedCache)) {
            appendToOutput(query.literal);
            return true;
        }
        return false;
    }

    bool InputCommand::trySymbolExtraction()
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
}