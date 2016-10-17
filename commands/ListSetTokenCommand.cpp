//
//  ListSetTokenCommand.cpp
//  jasl
//
//  Created by Ben Jones on 08/11/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "ListSetTokenCommand.hpp"
#include "../LiteralString.hpp"
#include "../VarExtractor.hpp"

namespace jasl
{
    ListSetTokenCommand::ListSetTokenCommand(Function &func_,
                                             SharedVarCache const &sharedCache,
                                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool ListSetTokenCommand::execute() 
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

    bool ListSetTokenCommand::getIndex(int64_t &index)
    {
        return VarExtractor::trySingleIntExtraction(m_func.paramA, index, m_sharedCache);
    }

    bool ListSetTokenCommand::getNewStringToken(std::string &value)
    {
        std::string token;
        if(!m_func.getValueC<std::string>(token, m_sharedCache)) {
            LiteralString literal;
            if(m_func.getValueC<LiteralString>(literal, m_sharedCache)) {
                value = literal.literal;
                return true;
            }
        } else {
            if(m_sharedCache->getVar_(token, value, Type::String)) {
                return true;
            }
        }
        setLastErrorMessage("get token: problem getting new token");
        return false;
    }

    bool ListSetTokenCommand::getNewVAToken(ValueArray &value)
    {
        std::string token;
        if(!m_func.getValueC<std::string>(token, m_sharedCache)) {
            if(m_func.getValueC<ValueArray>(value, m_sharedCache)) {
                return true;
            }
        } else {
            if(m_sharedCache->getVar_(token, value, Type::ValueArray)) {
                return true;
            }
        }
        setLastErrorMessage("get token: problem getting new token");
        return false;
    }

    bool ListSetTokenCommand::tryWithRawList(std::string const &varName) 
    {
        int64_t index;
        if(!getIndex(index)) {
            setLastErrorMessage("get token: error getting index");
            return false;
        }


        ValueArray v;
        if(m_func.getValueB<ValueArray>(v, m_sharedCache)) {
            if(index >= v.size()) {
                setLastErrorMessage("get token: index bigger than list");
                return false;
            }
            // string token
            {
                std::string newToken;
                if(getNewStringToken(newToken)) {
                    v[index] = Value(newToken);
                    m_sharedCache->setVar(varName, v, Type::ValueArray);
                    return true;
                }
            }
            // list token
            {
                ValueArray newToken;
                if(getNewVAToken(newToken)) {
                    v[index] = Value(newToken);
                    m_sharedCache->setVar(varName, v, Type::ValueArray);
                    return true;
                }
            }
        }

        setLastErrorMessage("get token: problem setting token");
        return false;
    }

    bool ListSetTokenCommand::tryWithSymbolList(std::string const &varName)
    {
        int64_t index;
        if(!getIndex(index)) {
            setLastErrorMessage("get token: error getting index");
            return false;
        }
        
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueB<std::string>(symbol, m_sharedCache)) {

            // find the ValueArray in the list cache having symbol symbol
            auto found = m_sharedCache->getVar<ValueArray>(symbol, Type::ValueArray);

            // if found then process list
            if(found) {

                if(index >= found->size()) {
                    setLastErrorMessage("get token: index bigger than list");
                    return false;
                }

                // if list to which token is being set is the same one as the variable
                // then update by reference else update by copy
                // string token
                {
                    std::string newToken;
                    if(getNewStringToken(newToken)) {
                        
                        if(symbol == varName) {

                            m_sharedCache->setTokenInList(varName, index, Value(newToken));
                            return true;
                        }

                        auto vals = *found;
                        vals[index] = Value(newToken);
                        m_sharedCache->setVar(varName, vals, Type::ValueArray);
                        return true;
                    }
                }
                // list token
                {
                    ValueArray newToken;
                    if(getNewVAToken(newToken)) {
                        
                        if(symbol == varName) {

                            m_sharedCache->setTokenInList(varName, index, Value(newToken));
                            return true;
                        }

                        auto vals = *found;
                        vals[index] = Value(newToken);
                        m_sharedCache->setVar(varName, vals, Type::ValueArray);
                        return true;
                    }
                }

            }
        }
        return false;
    }
}