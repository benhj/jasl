//
//  ListTokenIndexCommand.cpp
//  jasl
//
//  Created by Ben Jones on 08/11/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "ListTokenIndexCommand.hpp"
#include "core/LiteralString.hpp"
#include "core/RegisterCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <boost/lexical_cast.hpp>


bool jasl::ListTokenIndexCommand::m_registered = 
registerCommand<jasl::ListTokenIndexCommand>();


namespace jasl
{

    ListTokenIndexCommand::ListTokenIndexCommand(Function &func_,
                                                 SharedCacheStack const &sharedCache,
                                                 OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    ListTokenIndexCommand::~ListTokenIndexCommand() = default;

    std::vector<std::string> ListTokenIndexCommand::getCommandNames()
    {
        return {"index_of"};
    }

    bool ListTokenIndexCommand::execute() 
    {
        std::string varName;
        if(!m_func.getValueC<std::string>(varName, m_sharedCache)) {
            setLastErrorMessage("index_of: couldn't parse list");
            return false;
        }

        if(tryWithRawList(varName)) { return true; }
        if(tryWithSymbolList(varName)) { return true; }

        return false;
    }

    OptionalString ListTokenIndexCommand::getTestToken()
    {
        LiteralString literal;
        std::string testString;
        if(!m_func.getValueA<LiteralString>(literal, m_sharedCache)) {
            std::string symbol;
            if(!m_func.getValueA<std::string>(symbol, m_sharedCache)) {
                return OptionalString();
            } 

            return m_sharedCache->getVar<std::string>(symbol, Type::String);
        } else {
            return OptionalString(literal.literal);
        }
        return OptionalString();
    }

    bool ListTokenIndexCommand::tryWithRawList(std::string const &varName) 
    {
        auto testToken(getTestToken());
        if(!testToken) {
            return false;
        }

        List v;
        if(m_func.getValueB<List>(v, m_sharedCache)) {
            std::string s;
            try {
                int i = 0;
                for(auto & val : v) {
                    std::string tok;
                    if(!VarExtractor::tryAnyCast(tok, val)) {
                        return false;
                    }
                    if(tok == testToken) {
                        m_sharedCache->setVar(varName, (int64_t)i, Type::Int);
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

    bool ListTokenIndexCommand::tryWithSymbolList(std::string const &varName)
    {

        auto testToken(getTestToken());
        if(!testToken) {
            return false;
        }
        
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueB<std::string>(symbol, m_sharedCache)) {

            // find the List in the list cache having symbol symbol
            auto found = m_sharedCache->getVar<List>(symbol, Type::List);

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
                            m_sharedCache->setVar(varName, (int64_t)i, Type::Int);
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
}
