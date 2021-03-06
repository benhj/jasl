//
//  WildcardEqCommand.cpp
//  jasl
//
//  Copyright (c) 2018 Ben Jones. All rights reserved.
//

#include "WildcardEqCommand.hpp"
#include "WildcardMatch.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <string>

bool jasl::WildcardEqCommand::m_registered = 
registerCommand<jasl::WildcardEqCommand>();

namespace jasl
{
    WildcardEqCommand::WildcardEqCommand(Function &func_,
                                         SharedCacheStack const &sharedCache,
                                         OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    WildcardEqCommand::~WildcardEqCommand() = default;

    std::vector<std::string> WildcardEqCommand::getCommandNames()
    {
        return {"wildcard_eq"};
    }

    bool WildcardEqCommand::execute()
    {

        // Command syntax:
        // wildcard_eq (text, wildcard) -> result;

        std::string text;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramA, text, m_sharedCache)) {
            setLastErrorMessage("wildcard_eq: couldn't determine string to match");
            return false;
        }

        std::string wildcard;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramB, wildcard, m_sharedCache)) {
            setLastErrorMessage("wildcard_eq: couldn't determine matches string");
            return false;
        }

        std::string symbol;
        if(!m_func.getValueC<std::string>(symbol, m_sharedCache)) {
            setLastErrorMessage("wildcard_eq: couldn't dtermine symbol");
            return false;
        }

        auto const result = wildcardMatch(wildcard.c_str(), text.c_str());

        m_sharedCache->setVar(symbol, (int64_t)result, Type::Int);
        return true;
    }
}
