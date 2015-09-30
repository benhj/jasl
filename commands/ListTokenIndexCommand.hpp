//
//  ListTokenIndexCommand.cpp
//  jasl
//
//  Created by Ben Jones on 16/03/15
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
    class ListTokenIndexCommand : public Command
    {
    public:
        ListTokenIndexCommand(Function &func_,
                              OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {
        }

        bool execute() override
        {
            std::string varName;
            if(!m_func.getValueC<std::string>(varName)) {
                setLastErrorMessage("index: couldn't parse list");
                return false;
            }

            if(tryWithRawList(varName)) { return true; }
            if(tryWithSymbolList(varName)) { return true; }

            return false;
        }
    private:

        OptionalString getTestToken()
        {
            LiteralString literal;
            std::string testString;
            if(!m_func.getValueA<LiteralString>(literal)) {
                std::string symbol;
                if(!m_func.getValueA<std::string>(symbol)) {
                    return OptionalString();
                } 

                return VarCache::getString(symbol);
            } else {
                return OptionalString(literal.literal);
            }
            return OptionalString();
        }

        bool tryWithRawList(std::string const &varName) 
        {
            auto testToken(getTestToken());
            if(!testToken) {
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
                            return false;
                        }
                        if(tok == testToken) {
                            VarCache::setInt(varName, i);
                            return true;
                        }
                        ++i;
                    }
                    return false;
                } catch( boost::bad_lexical_cast const& ) {
                    return false;
                }
            }
            return false;
        }

        bool tryWithSymbolList(std::string const &varName)
        {

            auto testToken(getTestToken());
            if(!testToken) {
                return false;
            }
            
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueB<std::string>(symbol)) {

                // find the ValueArray in the list cache having symbol symbol
                auto found = VarCache::getList(symbol);

                // if found then process list
                if(found) {
                    std::string s;
                    try {
                        int i = 0;
                        for(auto & val : *found) {
                            std::string tok;
                            if(!VarExtractor::tryAnyCast(tok, val)) {
                                return false;
                            }
                            if(tok == testToken) {
                                VarCache::setInt(varName, i);
                                return true;
                            }
                            ++i;
                        }
                        return false;
                    } catch( boost::bad_lexical_cast const& ) {
                        return false;
                    }
                }
            }
           return false;
        }
    };

}