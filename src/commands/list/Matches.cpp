/// (c) 2019 Ben H. D. Jones

#include "Matches.hpp"
#include "caching/VarExtractor.hpp"
#include "commands/types/ReleaseCommand.hpp"
#include <boost/optional.hpp>
#include <ostream>

namespace jasl {

    List replaceAllHat(List const & orig, SharedCacheStack sharedCache)
    {
        List toReturn;
        std::string str;
        List list;
        for(auto const & element : orig) {
            auto strSuccess = VarExtractor::tryAnyCast(str, element);
            if(strSuccess) {
                int start = 0;
                if(str.length() >= 2 && str[0] == '^') {
                    start = 1;
                    if(str.length() >= 3 && str[1] == '^') {
                         ++start;
                    }
                }
                // some kind of hat
                if(start > 0) {
                    std::string var{std::begin(str) + start, std::end(str)};
                    {
                        auto const extracted = sharedCache->getVar<std::string>(var, Type::String);
                        if(extracted) {
                            toReturn.push_back(*extracted);
                        }
                    }
                    {
                        auto const extracted = sharedCache->getVar<List>(var, Type::List);
                        if(extracted) {
                            auto inner = replaceAllHat(*extracted, sharedCache);
                            if(start == 1) {
                                toReturn.push_back(inner);
                            } else {
                                for(auto const & it : inner) {
                                    toReturn.push_back(it);
                                }
                            }
                        }
                    }
                } else {
                    toReturn.push_back(element);
                }
            } else {
                toReturn.push_back(element);
            }
        }
        return toReturn;
    }

    void releasePreviousExtract(std::string const & var,
                                SharedCacheStack sharedCache)
    {
        Function f;
        f.name = "release";
        f.paramA = var;
        (void)ReleaseCommand(f, sharedCache, ::boost::optional<std::ostream&>()).execute();
    }

    MatchType Matches::doMatches(Value const & first,
                                 Value const & second) const
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

    bool Matches::handleExact(List const & first,
                              List const & second,
                              List::const_iterator & itFirst,
                              List::const_iterator & itSecond) const
    {
        ++itFirst;
        ++itSecond;

        if(itFirst != std::end(first) && itSecond == std::end(second)) {
            return false;
        }

        return true;
    }

    bool Matches::handleQVar(List const & first,
                             List const & second,
                             List::const_iterator & itFirst,
                             List::const_iterator & itSecond) const
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
                if(m_callCounter == 0) {
                    if(m_sharedCache->getVar<List>(var, Type::List)) {
                        releasePreviousExtract(var, m_sharedCache);
                    }

                    m_sharedCache->setVar(var, toStoreString, Type::String);
                } 
                // We previously stored a string at ?var
                // It needs to be removed (released) and a new list
                // needs to be added in its place with the original
                // string and the newly extracted one here.
                else if(m_callCounter == 1) {
                    auto const prev = m_sharedCache->getVar<std::string>(var, Type::String);
                    releasePreviousExtract(var, m_sharedCache);
                    List newList;
                    newList.push_back(*prev);
                    newList.push_back(toStoreString);
                    m_sharedCache->setVar(var, newList, Type::List);
                } 
                // Push back to already-existing list
                else {
                    m_sharedCache->pushBackTokenInList(var, Value(toStoreString));
                }
            } 
            // Try with list
            else {
                List list;
                if(VarExtractor::tryAnyCast(list, toStore)) {
                    if(m_callCounter == 0) {
                        if(m_sharedCache->getVar<List>(var, Type::List)) {
                            releasePreviousExtract(var, m_sharedCache);
                        }
                        m_sharedCache->setVar(var, list, Type::List);
                    } 
                    // See comment above in application to string extraction. Only difference
                    // here is that a list of lists is created.
                    else if(m_callCounter == 1) {
                        auto const prev = m_sharedCache->getVar<List>(var, Type::List);
                        releasePreviousExtract(var, m_sharedCache);
                        List newList;
                        newList.push_back(*prev);
                        newList.push_back(list);
                        m_sharedCache->setVar(var, newList, Type::List);
                    } else {
                         m_sharedCache->pushBackTokenInList(var, Value(list));
                    }
                } 
            }
        }
        return true;
    }

    bool Matches::handleQQVar(List const & first,
                              List const & second,
                              List::const_iterator & itFirst,
                              List::const_iterator & itSecond) const
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
            m_sharedCache->setVar(var, list, Type::List);
            return true;
        }

        // More token to process in first. We keep looping until we
        // find a match.
        auto matchType = doMatches(*itFirst, *itSecond);
        while(matchType == MatchType::NoMatch) {
            list.push_back(*itFirst);
            ++itFirst;
            if(itFirst == std::end(first)) {
                return false;
            }
            matchType = doMatches(*itFirst, *itSecond);
        }

        ++itFirst;
        ++itSecond;
        m_sharedCache->setVar(var, list, Type::List);
        return true;
    }

    bool Matches::handleDoubleEq(List const & first,
                                 List const & second,
                                 List::const_iterator & itFirst,
                                 List::const_iterator & itSecond) const
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
        while(doMatches(*itFirst, *itSecond) == MatchType::NoMatch) {
            ++itFirst;
            if(itFirst == std::end(first)) {
                return false;
            }
        }
        ++itFirst;
        ++itSecond;
        return true;
    }

    bool Matches::handleEq(List const & first,
                           List const & second,
                           List::const_iterator & itFirst,
                           List::const_iterator & itSecond) const
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

    Matches::Matches(SharedCacheStack sharedCache)
      : m_sharedCache(std::move(sharedCache))
      , m_callCounter(0)
    {}

    bool Matches::matches(List const & first,
                          List const & second) const
    {
        auto secondNoHats = replaceAllHat(second, m_sharedCache);
        auto const ismatch = doMatches(first, secondNoHats);
        if(ismatch) {
            ++m_callCounter;
        }
        return ismatch;
    }

    bool Matches::doMatches(List const & first,
                            List const & second) const
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
            auto match = doMatches(*itFirst, *itSecond);

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
                    if(handleQVar(first, second, itFirst, itSecond)) {
                        continue;
                    }
                    return false;
                    
                }
                case MatchType::QQVar:
                {
                    if(handleQQVar(first, second, itFirst, itSecond)) {
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
                        if(doMatches(listFirst, listSecond)) {
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
}