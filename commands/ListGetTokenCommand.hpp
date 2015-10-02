//
//  ListGetTokenCommand.cpp
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
    class ListGetTokenCommand : public Command
    {
    public:
        ListGetTokenCommand(Function &func_,
                            OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {
        }

        bool execute() override
        {
            std::string varName;
            if(!m_func.getValueC<std::string>(varName)) {
                setLastErrorMessage("get token: couldn't parse variable name");
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

        bool tryWithRawList(std::string const &varName) 
        {
            auto index(getIndex());
            if(!index) {
                setLastErrorMessage("get_token: error getting index");
                return false;
            }

            ValueArray v;
            if(m_func.getValueB<ValueArray>(v)) {
                std::string s;
                try {
                    int i = 0;
                    for(auto & val : v) {
                        if(i == *index) {
                            // First try pulling a string out
                            {
                                std::string tok;
                                if(VarExtractor::tryAnyCast(tok, val)) {
                                    VarCache::setString(varName, tok);
                                    return true;
                                }
                            }
                            // Second, try pulling ValueArray out (nb, a nested list)
                            {
                                ValueArray tok;
                                if(VarExtractor::tryAnyCast(tok, val)) {
                                    VarCache::setList(varName, tok);
                                    return true;
                                }
                            }

                            // Failed, set error state
                            setLastErrorMessage("get_token: problem extracting token");
                            return false;
                        }
                        
                        ++i;
                    }
                    setLastErrorMessage("get_token: problem extracting token");
                    return false;
                } catch( boost::bad_lexical_cast const& ) {
                    setLastErrorMessage("get token: error in lexical cast");
                    return false;
                }
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
            
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueB<std::string>(symbol)) {

                // find the ValueArray in the list cache having symbol symbol
                auto list = VarCache::getList(symbol);

                // if found then process list
                if(list) {
                    std::string s;
                    try {
                        int i = 0;
                        for(auto & val : *list) {
                            if(i == *index) {
                            // First try pulling a string out
                            {
                                std::string tok;
                                if(VarExtractor::tryAnyCast(tok, val)) {
                                    VarCache::setString(varName, tok);
                                    return true;
                                }
                            }
                            // Second, try pulling ValueArray out (nb, a nested list)
                            {
                                ValueArray tok;
                                if(VarExtractor::tryAnyCast(tok, val)) {
                                    VarCache::setList(varName, tok);
                                    return true;
                                }
                            }

                            // Failed, set error state
                            setLastErrorMessage("get_token: problem extracting token");
                            return false;
                        }
                            ++i;
                        }
                        setLastErrorMessage("get token: error getting list");
                        return false;
                    } catch( boost::bad_lexical_cast const& ) {
                        setLastErrorMessage("get token: error in lexical cast");
                        return false;
                    }
                }
            }
           return false;
        }
    };

}