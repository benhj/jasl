//
//  RegexCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "RegexCommand.hpp"
#include "../caching/VarExtractor.hpp"
#include <regex>
#include <string>

namespace jasl
{
    RegexCommand::RegexCommand(Function &func_,
                               SharedCacheStack const &sharedCache,
                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool RegexCommand::execute()
    {

        // Command syntax:
        // regex(toMatch, reg) -> result;

        std::string toMatch;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramA, toMatch, m_sharedCache)) {
            setLastErrorMessage("regex: couldn't determine string to match");
            return false;
        }

        std::string reg;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramB, reg, m_sharedCache)) {
            setLastErrorMessage("regex: couldn't determine regex string");
            return false;
        }

        std::string symbol;
        if(!m_func.getValueB<std::string>(symbol, m_sharedCache)) {
            setLastErrorMessage("regex: couldn't dtermine symbol");
            return false;
        }

        std::regex txtRegex(reg);
        auto const result = std::regex_match(toMatch, txtRegex);

        m_sharedCache->setVar(symbol, result, Type::Bool);
        return true;
    }
}
