//
//  RegexParseCommand.cpp
//  jasl
//
//  Copyright (c) 2018 Ben Jones. All rights reserved.
//

#include "RegexParseCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <regex>
#include <string>
#include <vector>

namespace {
    inline
    std::vector<std::string> extract(std::string const & text,
                                     std::string const & reg)
    {
        static std::regex const hl_regex(reg, std::regex_constants::icase) ;
        return { std::sregex_token_iterator( text.begin(), text.end(), hl_regex, 1 ),
                 std::sregex_token_iterator{} } ;
    }
}

namespace jasl
{
    RegexParseCommand::RegexParseCommand(Function &func_,
                                         SharedCacheStack const &sharedCache,
                                         OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool RegexParseCommand::execute()
    {

        // Command syntax:
        // regex_parse (text, reg) -> result;
        std::string text;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramA, text, m_sharedCache)) {
            setLastErrorMessage("regex_parse: couldn't determine string to parse");
            return false;
        }

        std::string reg;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramB, reg, m_sharedCache)) {
            setLastErrorMessage("regex_parse: couldn't determine regex_parse string");
            return false;
        }

        std::string symbol;
        if(!m_func.getValueC<std::string>(symbol, m_sharedCache)) {
            setLastErrorMessage("regex_parse: couldn't determine symbol");
            return false;
        }

        auto const result = extract(text, reg);

        m_sharedCache->setVar(symbol, result, Type::StringArray);
        return true;
    }
}
