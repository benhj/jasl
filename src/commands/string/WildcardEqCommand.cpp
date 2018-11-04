//
//  WildcardEqCommand.cpp
//  jasl
//
//  Copyright (c) 2018 Ben Jones. All rights reserved.
//

#include "WildcardEqCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <string>

namespace {
    // Not mine -- copied directly from
    // https://stackoverflow.com/questions/3300419/file-name-matching-with-wildcard
    bool match(char const *needle, char const *haystack) {
        for (; *needle != '\0'; ++needle) {
            switch (*needle) {
            case '?': 
                if (*haystack == '\0')
                    return false;
                ++haystack;
                break;
            case '*': {
                if (needle[1] == '\0')
                    return true;
                size_t max = strlen(haystack);
                for (size_t i = 0; i < max; i++)
                    if (match(needle + 1, haystack + i))
                        return true;
                return false;
            }
            default:
                if (*haystack != *needle)
                    return false;
                ++haystack;
            }
        }
        return *haystack == '\0';
    }
}

namespace jasl
{
    WildcardEqCommand::WildcardEqCommand(Function &func_,
                                         SharedCacheStack const &sharedCache,
                                         OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool WildcardEqCommand::execute()
    {

        // Command syntax:
        // wildcard_eq (text, wildcard) -> result;

        std::string text;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramA, text, m_sharedCache)) {
            setLastErrorMessage("wildcardex_eq: couldn't determine string to match");
            return false;
        }

        std::string wildcard;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramB, wildcard, m_sharedCache)) {
            setLastErrorMessage("wildcardex_eq: couldn't determine matches string");
            return false;
        }

        std::string symbol;
        if(!m_func.getValueC<std::string>(symbol, m_sharedCache)) {
            setLastErrorMessage("wildcardex_eq: couldn't dtermine symbol");
            return false;
        }

        auto const result = match(wildcard.c_str(), text.c_str());

        m_sharedCache->setVar(symbol, (int64_t)result, Type::Int);
        return true;
    }
}
