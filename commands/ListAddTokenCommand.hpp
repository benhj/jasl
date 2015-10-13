//
//  ListAddTokenCommand.cpp
//  jasl
//
//  Created by Ben Jones on 04/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../LiteralString.hpp"
#include "../VarExtractor.hpp"
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <sstream>
#include <cstdint>

namespace jasl
{
    class ListAddTokenCommand : public Command
    {
    public:
        ListAddTokenCommand(Function &func_,
                            SharedVarCache const &sharedCache = SharedVarCache(),
                            OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {
        }

        bool execute() override
        {
            std::string varName;
            if(!m_func.getValueB<std::string>(varName, m_sharedCache)) {
                setLastErrorMessage("add token: couldn't parse variable name");
                return false;
            }

            if(tryWithSymbolList(varName)) { return true; }
            setLastErrorMessage("add token: no list found");
            return false;
        }
    private:

        OptionalString getNewStringToken()
        {
            std::string token;
            if(!m_func.getValueA<std::string>(token, m_sharedCache)) {
                LiteralString literal;
                if(m_func.getValueA<LiteralString>(literal, m_sharedCache)) {
                    return OptionalString(literal.literal);
                }
            } else {
                auto result = m_sharedCache->getString(token);
                if(result) {
                    return OptionalString(*result);
                }
            }
            setLastErrorMessage("add token: problem getting new token");
            return OptionalString();
        }

        OptionalValueArray getNewVAToken()
        {
            std::string token;
            if(!m_func.getValueA<std::string>(token, m_sharedCache)) {
                ValueArray va;
                if(m_func.getValueA<ValueArray>(va, m_sharedCache)) {
                    return va;
                }
            } else {
                auto result = m_sharedCache->getList(token);
                if(result) {
                    return *result;
                }
            }
            setLastErrorMessage("add token: problem getting new token");
            return OptionalValueArray();
        }

        bool tryWithSymbolList(std::string const &varName)
        {
            // find the ValueArray in the list cache having symbol symbol
            auto found = m_sharedCache->getList(varName);

            // if found then process list
            if(found) {
                // if list to which token is being set is the same one as the variable
                // then update by reference else update by copy
                // string token
                {
                    auto newToken(getNewStringToken());
                    if(newToken) {
                        if(GlobalCache::getList(varName)) {
                            GlobalCache::pushBackTokenInList(varName, Value(*newToken));
                        }
                        m_sharedCache->pushBackTokenInList(varName, Value(*newToken));
                        return true;
                    }
                }
                // list token
                {
                    auto newToken(getNewVAToken());
                    if(newToken) {
                        if(GlobalCache::getList(varName)) {
                            GlobalCache::pushBackTokenInList(varName, Value(*newToken));
                        }
                        m_sharedCache->pushBackTokenInList(varName, Value(*newToken));
                        return true;
                    }
                }
            }
            return false;
        }
    };

}