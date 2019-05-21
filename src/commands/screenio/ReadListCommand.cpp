//
//  ReadListCommand.cpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#include "ReadListCommand.hpp"
#include "core/LiteralString.hpp"
#include "core/RegisterCommand.hpp"
#include "caching/VarExtractor.hpp"

bool jasl::ReadListCommand::m_registered = 
registerCommand<jasl::ReadListCommand>();

namespace {
    /// hacky as fuck. Should improve this.
    List split(const char *str, char c = ' ')
    {
        List result;
        do {
            const char *begin = str;
            while(*str != c && *str) {
                str++;
            }
            result.push_back(std::string(begin, str));
        } while (0 != *str++);
        return result;
    }
}

namespace jasl
{
    ReadListCommand::ReadListCommand(Function &func_,
                               SharedCacheStack const &sharedCache,
                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    ReadListCommand::~ReadListCommand() = default;

    std::vector<std::string> ReadListCommand::getCommandNames()
    {
        return {"readlist"};
    }

    bool ReadListCommand::execute()
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
        m_sharedCache->setVar(answer, split(result.c_str()), Type::List);
        return false;
    }

    bool ReadListCommand::tryLiteralExtraction() 
    {
        LiteralString query;
        if(m_func.getValueA<LiteralString>(query, m_sharedCache)) {
            appendToOutput(query.literal);
            return true;
        }
        return false;
    }

    bool ReadListCommand::trySymbolExtraction()
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
