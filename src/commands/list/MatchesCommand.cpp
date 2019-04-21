//
//  MatchesCommand.cpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#include "MatchesCommand.hpp"
#include "Matches.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"

bool jasl::MatchesCommand::m_registered = 
registerCommand<jasl::MatchesCommand>();

namespace jasl
{
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
