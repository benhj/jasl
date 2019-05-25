//
//  ListGetTokenCommand.cpp
//  jasl
//
//  Created by Ben Jones on 08/11/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "ListGetTokenCommand.hpp"
#include "core/LiteralString.hpp"
#include "core/RegisterCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <boost/lexical_cast.hpp>

bool jasl::ListGetTokenCommand::m_registered = 
registerCommand<jasl::ListGetTokenCommand>();

namespace jasl
{
    ListGetTokenCommand::ListGetTokenCommand(Function &func_,
                                             SharedCacheStack const &sharedCache,
                                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    ListGetTokenCommand::~ListGetTokenCommand() = default;

    std::vector<std::string> ListGetTokenCommand::getCommandNames()
    {
        return {"get_token"};
    }

    bool ListGetTokenCommand::execute()
    {
        std::string varName;
        if(!m_func.getValueC<std::string>(varName, m_sharedCache)) {
            setLastErrorMessage("get_token: couldn't parse variable name");
            return false;
        }

        if(tryWithRawList(varName)) { return true; }
        if(tryWithSymbolList(varName)) { return true; }
        setLastErrorMessage("get_token: no list found");
        return false;
    }

    bool ListGetTokenCommand::getIndex(int64_t &value)
    {
        return VarExtractor::trySingleIntExtraction(m_func.paramA, value, m_sharedCache);
    }

    bool ListGetTokenCommand::tryWithRawList(std::string const &varName) 
    {
        int64_t index;
        if(!getIndex(index)) {
            setLastErrorMessage("get_token: error getting index");
            return false;
        }

        List v;
        if(m_func.getValueB<List>(v, m_sharedCache)) {
            std::string s;
            try {
                int i = 0;
                for(auto & val : v) {
                    if(i == index) {
                        // First try pulling a string out
                        {
                            std::string tok;
                            if(VarExtractor::tryAnyCast(tok, val)) {
                                m_sharedCache->setVar(varName, tok, Type::String);
                                return true;
                            }
                        }
                        // Second, try pulling List out (nb, a nested list)
                        {
                            List tok;
                            if(VarExtractor::tryAnyCast(tok, val)) {
                                m_sharedCache->setVar(varName, tok, Type::List);
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
                setLastErrorMessage("get_token: error in lexical cast");
                return false;
            }
        }
        return false;
    }

    bool ListGetTokenCommand::tryWithSymbolList(std::string const &varName)
    {
        int64_t index;
        if(!getIndex(index)) {
            setLastErrorMessage("get_token: error getting index");
            return false;
        }

        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueB<std::string>(symbol, m_sharedCache)) {

            // find the List in the list cache having symbol symbol
            auto list = m_sharedCache->getVar<List>(symbol, Type::List);

            // if found then process list
            if(list) {
                std::string s;
                try {
                    int i = 0;
                    for(auto & val : *list) {
                        if(i == index) {
                        // First try pulling a string out
                        {
                            std::string tok;
                            if(VarExtractor::tryAnyCast(tok, val)) {
                                m_sharedCache->setVar(varName, tok, Type::String);
                                return true;
                            }
                        }
                        // Second, try pulling List out (nb, a nested list)
                        {
                            List tok;
                            if(VarExtractor::tryAnyCast(tok, val)) {
                                m_sharedCache->setVar(varName, tok, Type::List);
                                return true;
                            }
                        }

                        // Failed, set error state
                        setLastErrorMessage("get_token: problem extracting token");
                        return false;
                    }
                        ++i;
                    }
                    setLastErrorMessage("get_token: error getting list");
                    return false;
                } catch( boost::bad_lexical_cast const& ) {
                    setLastErrorMessage("get_token: error in lexical cast");
                    return false;
                }
            }
        }
       return false;
    }
}
