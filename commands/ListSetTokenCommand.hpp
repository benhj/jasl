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
#include "../VarCache.hpp"
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
                            OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {
        }

        bool execute() override
        {
            std::string varName;
            if(!m_func.getValueD<std::string>(varName)) {
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
            return VarExtractor::trySingleIntExtraction(m_func.paramA);
        }

        OptionalString getNewToken()
        {
            std::string token;
            if(!m_func.getValueC<std::string>(token)) {
                LiteralString literal;
                if(m_func.getValueC<LiteralString>(literal)) {
                    return OptionalString(literal.literal);
                }
            } else {
                auto result = VarExtractor::searchString(token);
                if(result) {
                    return OptionalString(*result);
                }
            }
            setLastErrorMessage("get token: problem getting new token");
            return OptionalString();
        }

        bool tryWithRawList(std::string const &varName) 
        {
            auto index(getIndex());
            if(!index) {
                setLastErrorMessage("get token: error getting index");
                return false;
            }

            auto newToken(getNewToken());
            if(!newToken) {
                setLastErrorMessage("get token: error getting new token");
                return false;
            }

            ValueArray v;
            if(m_func.getValueB<ValueArray>(v)) {
                if(*index >= v.size()) {
                    setLastErrorMessage("get token: index bigger than list");
                    return false;
                }
                v[*index] = Value(*newToken);
                VarCache::listCache[varName] = v;
                return true;
            }
            return false;
        }

        bool tryWithSymbolList(std::string const &varName)
        {
            auto index(getIndex());
            if(!index) {
                setLastErrorMessage("get token: error getting index");
                return false;
            }

            auto newToken(getNewToken());
            if(!newToken) {
                setLastErrorMessage("get token: error getting new token");
                return false;
            }
            
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueB<std::string>(symbol)) {

                // find the ValueArray in the listCache having symbol symbol
                auto it = VarCache::listCache.find(symbol);

                // if found then process list
                if(it != std::end(VarCache::listCache)) {

                    if(*index >= it->second.size()) {
                        setLastErrorMessage("get token: index bigger than list");
                        return false;
                    }

                    // if list to which token is being set is the same one as the variable
                    // then update by reference else update by copy
                    if(symbol == varName) {
                        VarCache::listCache[varName][*index] = Value(*newToken);
                        return true;
                    }

                    auto vals = it->second;
                    vals[*index] = Value(*newToken);
                    VarCache::listCache[varName] = vals;
                    return true;

                }
            }
           return false;
        }
    };

}