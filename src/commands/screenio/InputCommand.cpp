//
//  InputCommand.cpp
//  jasl
//
//  Created by Ben Jones on 8/11/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "InputCommand.hpp"
#include "core/LiteralString.hpp"
#include "core/RegisterCommand.hpp"
#include "caching/VarExtractor.hpp"

bool jasl::InputCommand::m_registered = 
registerCommand<jasl::InputCommand>();

namespace jasl
{
    InputCommand::InputCommand(Function &func_,
                               SharedCacheStack const &sharedCache,
                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    std::vector<std::string> InputCommand::getCommandNames()
    {
        return {"input"};
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
        m_sharedCache->setVar(answer, result, Type::String);
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
                auto result = m_sharedCache->getVar<std::string>(symbol, Type::String);
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