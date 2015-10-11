//
//  ListSetTokenCommand.cpp
//  jasl
//
//  Created by Ben Jones on 18/03/15
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
    class ListSetTokenCommand : public Command
    {
    public:
        ListSetTokenCommand(Function &func_,
                            SharedVarCache const &sharedCache = SharedVarCache(),
                            OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {
        }

        bool execute() override
        {
            std::string varName;
            if(!m_func.getValueD<std::string>(varName, m_sharedCache)) {
                setLastErrorMessage("set token: couldn't parse variable name");
                return false;
            }

            if(tryWithRawList(varName)) { return true; }
            if(tryWithSymbolList(varName)) { return true; }
            setLastErrorMessage("get token: no list found");
            return false;
        }
    private:

        OptionalInt getIndex()
        {
            return VarExtractor::trySingleIntExtraction(m_func.paramA, m_sharedCache);
        }

        OptionalString getNewStringToken()
        {
            std::string token;
            if(!m_func.getValueC<std::string>(token, m_sharedCache)) {
                LiteralString literal;
                if(m_func.getValueC<LiteralString>(literal, m_sharedCache)) {
                    return OptionalString(literal.literal);
                }
            } else {
                auto result = m_sharedCache->getString(token);
                if(result) {
                    return OptionalString(*result);
                }
            }
            setLastErrorMessage("get token: problem getting new token");
            return OptionalString();
        }

        OptionalValueArray getNewVAToken()
        {
            std::string token;
            if(!m_func.getValueC<std::string>(token, m_sharedCache)) {
                ValueArray va;
                if(m_func.getValueC<ValueArray>(va, m_sharedCache)) {
                    return va;
                }
            } else {
                auto result = m_sharedCache->getList(token);
                if(result) {
                    return *result;
                }
            }
            setLastErrorMessage("get token: problem getting new token");
            return OptionalValueArray();
        }

        bool tryWithRawList(std::string const &varName) 
        {
            auto index(getIndex());
            if(!index) {
                setLastErrorMessage("get token: error getting index");
                return false;
            }


            ValueArray v;
            if(m_func.getValueB<ValueArray>(v, m_sharedCache)) {
                if(*index >= v.size()) {
                    setLastErrorMessage("get token: index bigger than list");
                    return false;
                }
                // string token
                {
                    auto newToken(getNewStringToken());
                    if(newToken) {
                        v[*index] = Value(*newToken);
                        m_sharedCache->setList(varName, v);
                        return true;
                    }
                }
                // list token
                {
                    auto newToken(getNewVAToken());
                    if(newToken) {
                        v[*index] = Value(*newToken);
                        m_sharedCache->setList(varName, v);
                        return true;
                    }
                }
            }

            setLastErrorMessage("get token: problem setting token");
            return false;
        }

        bool tryWithSymbolList(std::string const &varName)
        {
            auto index(getIndex());
            if(!index) {
                setLastErrorMessage("get token: error getting index");
                return false;
            }
            
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueB<std::string>(symbol, m_sharedCache)) {

                // find the ValueArray in the list cache having symbol symbol
                auto found = m_sharedCache->getList(symbol);

                // if found then process list
                if(found) {

                    if(*index >= found->size()) {
                        setLastErrorMessage("get token: index bigger than list");
                        return false;
                    }

                    // if list to which token is being set is the same one as the variable
                    // then update by reference else update by copy
                    // string token
                    {
                        auto newToken(getNewStringToken());
                        if(newToken) {
                            
                            if(symbol == varName) {

                                m_sharedCache->setTokenInList(varName, *index, Value(*newToken));
                                return true;
                            }

                            auto vals = *found;
                            vals[*index] = Value(*newToken);
                            m_sharedCache->setList(varName, vals);
                            return true;
                        }
                    }
                    // list token
                    {
                        auto newToken(getNewVAToken());
                        if(newToken) {
                            
                            if(symbol == varName) {

                                m_sharedCache->setTokenInList(varName, *index, Value(*newToken));
                                return true;
                            }

                            auto vals = *found;
                            vals[*index] = Value(*newToken);
                            m_sharedCache->setList(varName, vals);
                            return true;
                        }
                    }

                }
            }
            return false;
        }
    };

}