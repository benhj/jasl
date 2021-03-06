//
//  ListToStringCommand.cpp
//  jasl
//
//  Created by Ben Jones on 08/11/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "ListToStringCommand.hpp"
#include "core/LiteralString.hpp"
#include "core/RegisterCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <boost/lexical_cast.hpp>

namespace jasl
{
    ListToStringCommand::ListToStringCommand(Function &func_,
                                             SharedCacheStack const &sharedCache,
                                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    ListToStringCommand::~ListToStringCommand() = default;

    std::vector<std::string> ListToStringCommand::getCommandNames()
    {
        return {"list_to_string"};
    }

    bool ListToStringCommand::execute()
    {
        std::string varName;
        if(!m_func.getValueB<std::string>(varName, m_sharedCache)) {
            setLastErrorMessage("list_to_string: couldn't parse list");
            return false;
        }

        if(tryRawListExtraction(varName)) { return true; }
        if(trySymbolExtraction(varName)) { return true; }

        return false;
    }

    bool ListToStringCommand::tryRawListExtraction(std::string const &varName) 
    {
        List v;
        if(m_func.getValueA<List>(v, m_sharedCache)) {
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
                m_sharedCache->setVar(varName, s, Type::String);
                return true;
            } catch( boost::bad_lexical_cast const& ) {
                setLastErrorMessage("list_to_string: couldn't parse list");
                return false;
            }
        }
        return false;
    }

    bool ListToStringCommand::trySymbolExtraction(std::string const &varName)
    {
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {

            // find the List in the list cache having symbol symbol
            auto found = m_sharedCache->getVar<List>(symbol, Type::List);

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
                    m_sharedCache->setVar(varName, s, Type::String);
                    return true;
                } catch( boost::bad_lexical_cast const& ) {
                    setLastErrorMessage("list_to_string: couldn't parse list");
                    return false;
                }
            }
        }
       return false;
    }
}
