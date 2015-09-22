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
                setLastErrorMessage("get token: error getting index");
                return false;
            }

            ValueArray v;
            if(m_func.getValueB<ValueArray>(v)) {
                std::string s;
                try {
                    int i = 0;
                    for(auto & val : v) {
                        std::string tok;
                        if(!VarExtractor::tryAnyCast(tok, val)) {
                            setLastErrorMessage("get token: error getting list token");
                            return false;
                        }
                        if(i == *index) {
                            VarCache::setString(varName, tok);
                            return true;
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

                // find the ValueArray in the listCache having symbol symbol
                auto it = VarCache::listCache.find(symbol);

                // if found then process list
                if(it != std::end(VarCache::listCache)) {
                    std::string s;
                    try {
                        int i = 0;
                        for(auto & val : it->second) {
                            std::string tok;
                            if(!VarExtractor::tryAnyCast(tok, val)) {
                                setLastErrorMessage("get token: error getting list token");
                                return false;
                            }
                            if(i == *index) {
                                VarCache::setString(varName, tok);
                                return true;
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