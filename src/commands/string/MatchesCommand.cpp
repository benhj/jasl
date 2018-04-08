//
//  MatchesCommand.cpp
//  jasl
//
//  Copyright (c) 2017-present Ben Jones. All rights reserved.
//

#include "MatchesCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <regex>
#include <string>

namespace jasl
{
    MatchesCommand::MatchesCommand(Function &func_,
                                  SharedCacheStack const &sharedCache,
                                  OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool MatchesCommand::execute()
    {

        // Command syntax:
        // matches(toMatch, reg) -> result;

        std::string toMatch;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramA, toMatch, m_sharedCache)) {
            setLastErrorMessage("matches: couldn't determine string to match");
            return false;
        }

        std::string reg;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramB, reg, m_sharedCache)) {
            setLastErrorMessage("matches: couldn't determine matches string");
            return false;
        }

        std::string symbol;
        if(!m_func.getValueC<std::string>(symbol, m_sharedCache)) {
            setLastErrorMessage("matches: couldn't dtermine symbol");
            return false;
        }

        std::regex txtRegex(reg);
        auto const result = std::regex_match(toMatch, txtRegex);

        m_sharedCache->setVar(symbol, (int64_t)result, Type::Int);
        return true;
    }
}
