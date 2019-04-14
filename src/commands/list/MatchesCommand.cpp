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
        std::string strFirst;
        std::string strSecond;

        // Initially try string extraction
        auto const strSuccessFirst = VarExtractor::tryAnyCast(strFirst, first);
        auto const strSuccessSecond = VarExtractor::tryAnyCast(strSecond, second);
        if(strSuccessFirst && strSuccessSecond) {
            if(strFirst == strSecond) {
                return MatchType::Exact;
            } 
        } 

        // Second string needed to have been successful
        if(strSuccessSecond) {
            if(strSecond == "=") {
                return MatchType::SingleEq;
            } else if(strSecond == "==") {
                return MatchType::DoubleEq;
            } else if(strSecond.at(0) == '?') {
                if(strSecond.size() > 1 && strSecond.at(1) == '?') {
                    return MatchType::QQVar;
                }
                return MatchType::QVar;
            }
        }
        return MatchType::NoMatch;
    }

    bool handleExact(List const & first,
                     List const & second,
                     List::const_iterator & itFirst,
                     List::const_iterator & itSecond)
    {
        ++itFirst;
        ++itSecond;

        if(itFirst != std::end(first) && itSecond == std::end(second)) {
            return false;
        }

        return true;
    }

    bool handleQVar(List const & first,
                    List const & second,
                    List::const_iterator & itFirst,
                    List::const_iterator & itSecond,
                    SharedCacheStack const & sharedCache)
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
            }
        }
        return true;
    }

    bool handleQQVar(List const & first,
                     List const & second,
                     List::const_iterator & itFirst,
                     List::const_iterator & itSecond,
                     SharedCacheStack const & sharedCache)
    {
        // Given the MatchType, *itSecond must be a string,
        // there is no way the cast to a string can fail.
        std::string qvar;
        (void)VarExtractor::tryAnyCast(qvar, *itSecond);
        std::string var{std::begin(qvar) + 2, std::end(qvar)};

        ++itSecond;

        // Need to create a list of elements
        List list;
        list.push_back(*itFirst);
        ++itFirst;

        // If we are at the end of the second string, as in
        // [end of string ??var] it means that even if there are
        // more tokens in the first string, we match them all
        // given '??var' so can simply store all tokens in the
        // list and return true.
        if(itSecond == std::end(second)) {
            while(itFirst != std::end(first)) {
                list.push_back(*itFirst);
                ++itFirst;
            }
            sharedCache->setVar(var, list, Type::List);
            return true;
        }

        // More token to process in first. We keep looping until we
        // find a match.
        while(matches(*itFirst, *itSecond) == MatchType::NoMatch) {
            list.push_back(*itFirst);
            ++itFirst;
            if(itFirst == std::end(first)) {
                return false;
            }
        }
        sharedCache->setVar(var, list, Type::List);
        return true;
    }

    bool handleDoubleEq(List const & first,
                        List const & second,
                        List::const_iterator & itFirst,
                        List::const_iterator & itSecond)
    {

        ++itSecond;
        ++itFirst;

        // If we are at the end of the second string, as in
        // [end of string ==] it means that even if there are
        // more tokens in the first string, we match them all
        // given '=='.
        if(itSecond == std::end(second)) {
            return true;
        }

        // More token to process in first. We keep looping until we
        // find a match.
        while(matches(*itFirst, *itSecond) == MatchType::NoMatch) {
            ++itFirst;
            if(itFirst == std::end(first)) {
                return false;
            }
        }
        return true;
    }

    bool handleEq(List const & first,
                  List const & second,
                  List::const_iterator & itFirst,
                  List::const_iterator & itSecond)
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

        return true;
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
        auto itFirst = first.begin();
        auto itSecond = second.begin();

        while(itFirst != std::end(first) && itSecond != std::end(second)) {

            // Discover type of token match
            auto match = matches(*itFirst, *itSecond);

            switch(match) {
                case MatchType::Exact:
                {
                    if(handleExact(first, second, itFirst, itSecond)) {
                        continue;
                    }
                    return false;
                }
                case MatchType::SingleEq:
                {
                    if(handleEq(first, second, itFirst, itSecond)) {
                        continue;
                    }
                    return false;
                }
                case MatchType::DoubleEq:
                {
                    if(handleDoubleEq(first, second, itFirst, itSecond)) {
                        continue;
                    }
                    return false;
                }
                case MatchType::QVar:
                {
                    if(handleQVar(first, second, itFirst, itSecond, sharedCache)) {
                        continue;
                    }
                    return false;
                    
                }
                case MatchType::QQVar:
                {
                    if(handleQQVar(first, second, itFirst, itSecond, sharedCache)) {
                        continue;
                    }
                    return false;
                }
                case MatchType::NoMatch:
                {
                    // Edge case in which one must be a list rather than a string
                    List listFirst;
                    List listSecond;
                    auto const listSuccessFirst = VarExtractor::tryAnyCast(listFirst, *itFirst);
                    auto const listSuccessSecond = VarExtractor::tryAnyCast(listSecond, *itSecond);
                    if(listSuccessFirst && listSuccessSecond) {
                        if(matches(listFirst, listSecond, sharedCache)) {
                            ++itFirst;
                            ++itSecond;
                            continue;
                        }
                    }
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
