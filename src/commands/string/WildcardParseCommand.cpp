//
//  WildcardParseCommand.cpp
//  jasl
//
//  Copyright (c) 2018 Ben Jones. All rights reserved.
//

#include "WildcardMatch.hpp"
#include "WildcardParseCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <string>
#include <vector>

namespace jasl
{
    WildcardParseCommand::WildcardParseCommand(Function &func_,
                                               SharedCacheStack const &sharedCache,
                                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool WildcardParseCommand::execute()
    {

        // Command syntax:
        // wildcard_parse (stringArray, wildcard) -> result;
        StringArray value;
        if(!VarExtractor::trySingleArrayExtraction(m_func.paramA, value, m_sharedCache, Type::StringArray)) {
            setLastErrorMessage("wildcard_parse: problem with string array");
            return false;
        }

        std::string wildcard;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramB, wildcard, m_sharedCache)) {
            setLastErrorMessage("wildcard_parse: couldn't determine wildcard_parse string");
            return false;
        }

        std::string symbol;
        if(!m_func.getValueC<std::string>(symbol, m_sharedCache)) {
            setLastErrorMessage("wildcard_parse: couldn't determine symbol");
            return false;
        }

        StringArray processed;
        for(auto const & c : value) {
            if(wildcardMatch(wildcard.c_str(), c.c_str())) {
                processed.push_back(c);
            }
        }

        m_sharedCache->setVar(symbol, processed, Type::StringArray);
        return true;
    }
}
