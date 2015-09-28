//
//  ListToStringCommand.cpp
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
    class ListToStringCommand : public Command
    {
    public:
        ListToStringCommand(Function &func_,
                                 OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {
        }

        bool execute() override
        {
            std::string varName;
            if(!m_func.getValueB<std::string>(varName)) {
                setLastErrorMessage("list_to_string: couldn't parse list");
                return false;
            }

            if(tryRawListExtraction(varName)) { return true; }
            if(trySymbolExtraction(varName)) { return true; }

            return false;
        }
    private:
        bool tryRawListExtraction(std::string const &varName) 
        {
            ValueArray v;
            if(m_func.getValueA<ValueArray>(v)) {
                std::string s;
                try {
                    for(auto & val : v) {
                        std::string tok;
                        if(!VarExtractor::tryAnyCast(tok, val)) {
                            return false;
                        }
                        // tokens separated by a single space
                        s.append(tok);
                        s.append(" ");
                    }
                    // remove last space
                    s.pop_back();
                    
                    // now finally store string in cache
                    VarCache::setString(varName, s);
                    return true;
                } catch( boost::bad_lexical_cast const& ) {
                    setLastErrorMessage("list_to_string: couldn't parse list");
                    return false;
                }
            }
            return false;
        }

        bool trySymbolExtraction(std::string const &varName)
        {
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol)) {

                // find the ValueArray in the list cache having symbol symbol
                auto found = VarCache::getList(symbol);

                // if found then process list
                if(found) {
                    std::string s;
                    try {
                        for(auto & val : *found) {
                            std::string tok;
                            if(!VarExtractor::tryAnyCast(tok, val)) {
                                return false;
                            }
                            // tokens separated by a single space
                            s.append(tok);
                            s.append(" ");
                        }
                        // remove last space
                        s.pop_back();

                        // now finally store string in cache
                        VarCache::setString(varName, s);
                        return true;
                    } catch( boost::bad_lexical_cast const& ) {
                        setLastErrorMessage("list_to_string: couldn't parse list");
                        return false;
                    }
                }
            }
           return false;
        }

        
    };

}