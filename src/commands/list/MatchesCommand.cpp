//
//  MatchesCommand.cpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#include "MatchesCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"

bool jasl::MatchesCommand::m_registered = 
registerCommand<jasl::MatchesCommand>();

namespace jasl
{
    // To describe how list tokens match.
    enum class MatchType
    {
        Exact,     // [hello], [hello]
        QVar,      // [hello], [?var]
        QQVar,     // [hello], [??var]
        SingleEq,  // [hello], [=]
        DoubleEq,  // [hello], [==]
        NoMatch    // [hello], [goodbye]
    };

    MatchType matches(Value const & first,
                      Value const & second)
    {
        // Note, no nesting permitted in this first implementation.
        std::string strFirst;
        std::string strSecond;
        (void)VarExtractor::tryAnyCast(strFirst, first);
        (void)VarExtractor::tryAnyCast(strSecond, second);
        if(strFirst == strSecond) {
            return MatchType::Exact;
        } else if(strSecond == "=") {
            return MatchType::SingleEq;
        } else if(strSecond == "==") {
            return MatchType::DoubleEq;
        } else if(strSecond.at(0) == '?') {
            if(strSecond.size() > 1 && strSecond.at(1) == '?') {
                return MatchType::QQVar;
            }
            return MatchType::QVar;
        }  
        return MatchType::NoMatch;
    }

    bool matches(List const & first,
                 List const & second,
                 SharedCacheStack const & sharedCache)
    {

        // Edge-case 1. Two empty string always match.
        if(first.empty() && second.empty()) {
            return true;
        }

        // Edge-case 2. If one string is empty, there
        // is no match.
        if(first.empty() || second.empty()) {
            return false;
        }

        // Edge-case 3. If the second string is longer, then
        // there is no match.
        if(second.size() > first.size()) {
            return false;
        }

        // Edge-case 4. If the second string is simply [==],
        // it matches the whole of the first string so we
        // can return early.
        std::string val;
        if(VarExtractor::tryAnyCast(val, second.at(0))) {
            if(second.size() == 1 && val == "==") {
                return true;
            }
        }

        // Loop through tokens in first and see if tokens in second match.
        auto itFirst = std::begin(first);
        auto itSecond = std::begin(second);

        while(itFirst != std::end(first) && itSecond != std::end(second)) {

            // Discover type of token match
            auto match = matches(*itFirst, *itSecond);

            switch(match) {
                case MatchType::Exact:
                {
                    ++itFirst;
                    ++itSecond;

                    if(itFirst != std::end(first) && itSecond == std::end(second)) {
                        return false;
                    }

                    continue;
                }
                case MatchType::SingleEq:
                {
                    ++itFirst;
                    ++itSecond;

                    // Boundary -- check if there are more tokens
                    // in list 2 to be processed. If so, we have come
                    // to the end of the first string and the overall match
                    // has failed.
                    if(itFirst != std::end(first) && itSecond == std::end(second)) {
                        return false;
                    }

                    continue;
                }
                case MatchType::DoubleEq:
                {
                    ++itSecond;

                    // If we are at the end of the second string, as in
                    // [end of string ==] it means that even if there are
                    // more tokens in the first string, we match them all
                    // given '==' so can simply return true.
                    if(itSecond == std::end(second)) {
                        return true;
                    }

                    // More token to process in first. We keep looping until we
                    // find a match.
                    ++itFirst;
                    while(!matches(List{itFirst, std::end(first)},
                                   List{itSecond, std::end(second)},
                                   sharedCache)) {
                        ++itFirst;

                        // If no more possible tokens to match in the first string,
                        // we have come to the end and must return false.
                        if(itFirst == std::end(first)) {
                            return false;
                        }
                    }
                    return true;
                }
                case MatchType::QVar:
                {
                    auto toStore = *itFirst;
                    // Given the MatchType, *itSecond must be a string,
                    // there is no way the cast to a string can fail.
                    std::string qvar;
                    (void)VarExtractor::tryAnyCast(qvar, *itSecond);
                    ++itFirst;
                    ++itSecond;
                    // Boundary -- check if there are more tokens
                    // in list 2 to be processed. If so, we have come
                    // to the end of the first string and the overall match
                    // has failed.
                    if(itFirst != std::end(first) && itSecond == std::end(second)) {
                        return false;
                    }
                    
                    // Now pull out variable
                    std::string var{std::begin(qvar) + 1, std::end(qvar)};
                    
                    // Pull out the data to be stored. Try string.
                    {
                        std::string toStoreString;
                        if(VarExtractor::tryAnyCast(toStoreString, toStore)) {
                            sharedCache->setVar(var, toStoreString, Type::String);
                            continue;
                        }
                    }
                    continue;
                    
                }
                case MatchType::QQVar:
                case MatchType::NoMatch:
                {
                    return false;
                }
            }
        }
        return true;
    }

    MatchesCommand::MatchesCommand(Function &func_,
                                   SharedCacheStack const &sharedCache,
                                   OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    MatchesCommand::~MatchesCommand() = default;

    std::vector<std::string> MatchesCommand::getCommandNames()
    {
        return {"matches"};
    }

    bool MatchesCommand::execute() 
    {
        // Try and extract List 1
        List list;
        if(!m_func.getValueA<decltype(list)>(list, m_sharedCache)) {
            std::string varName;
            if(m_func.getValueA<decltype(varName)>(varName, m_sharedCache)) {
                auto theList = m_sharedCache->getVar<List>(varName, Type::List);
                if(theList) {
                    list = *theList;
                }
            } else {
                setLastErrorMessage("matches: error");
                return false;
            }
        }

        // Now try and extract List 2
        List list2;
        if(!m_func.getValueB<decltype(list2)>(list2, m_sharedCache)) {
            std::string varName;
            if(m_func.getValueB<decltype(varName)>(varName, m_sharedCache)) {
                auto theList = m_sharedCache->getVar<List>(varName, Type::List);
                if(theList) {
                    list2 = *theList;
                }
            } else {
                setLastErrorMessage("matches: error");
                return false;
            }
        }

        std::string resultName;
        if(!m_func.getValueC<std::string>(resultName, m_sharedCache)) {
            setLastErrorMessage("matches: error");
            return false;
        }

        auto const matchesResult = matches(list, list2, m_sharedCache);

        m_sharedCache->setVar(resultName, matchesResult, Type::Bool);
        return true;
    }
}
