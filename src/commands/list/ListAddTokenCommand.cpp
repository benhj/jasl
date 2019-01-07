//
//  ListAddTokenCommand.cpp
//  jasl
//
//  Created by Ben Jones on 08/11/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "ListAddTokenCommand.hpp"
#include "core/LiteralString.hpp"
#include "core/RegisterCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <sstream>

namespace jasl
{
    ListAddTokenCommand::ListAddTokenCommand(Function &func_,
                                             SharedCacheStack const &sharedCache,
                                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    std::vector<std::string> ListAddTokenCommand::getCommandNames()
    {
        return {"add_token"};
    }

    bool ListAddTokenCommand::execute()
    {
        std::string varName;
        if(!m_func.getValueB<std::string>(varName, m_sharedCache)) {
            setLastErrorMessage("list_add: couldn't parse variable name");
            return false;
        }

        if(tryWithSymbolList(varName)) { return true; }
        setLastErrorMessage("list_add: no list found");
        return false;
    }

    OptionalString ListAddTokenCommand::getNewStringToken()
    {
        std::string token;
        if(!m_func.getValueA<std::string>(token, m_sharedCache)) {
            LiteralString literal;
            if(m_func.getValueA<LiteralString>(literal, m_sharedCache)) {
                return OptionalString(literal.literal);
            }
        } else {
            auto result = m_sharedCache->getVar<std::string>(token, Type::String);
            if(result) {
                return OptionalString(*result);
            }
        }
        setLastErrorMessage("list_add: problem getting new token");
        return OptionalString();
    }

    OptionalList ListAddTokenCommand::getNewVAToken()
    {
        std::string token;
        if(!m_func.getValueA<std::string>(token, m_sharedCache)) {
            List va;
            if(m_func.getValueA<List>(va, m_sharedCache)) {
                return va;
            }
        } else {
            auto result = m_sharedCache->getVar<List>(token, Type::List);
            if(result) {
                return *result;
            }
        }
        setLastErrorMessage("list_add: problem getting new token");
        return OptionalList();
    }

    bool ListAddTokenCommand::tryWithSymbolList(std::string const &varName)
    {
        // find the List in the list cache having symbol symbol
        auto found = m_sharedCache->getVar<List>(varName, Type::List);

        // if found then process list
        if(found) {
            // if list to which token is being set is the same one as the variable
            // then update by reference else update by copy
            // string token
            {
                auto newToken(getNewStringToken());
                if(newToken) {
                    m_sharedCache->pushBackTokenInList(varName, Value(*newToken));
                    return true;
                }
            }
            // list token
            {
                auto newToken(getNewVAToken());
                if(newToken) {
                    m_sharedCache->pushBackTokenInList(varName, Value(*newToken));
                    return true;
                }
            }
        }
        return false;
    }
}