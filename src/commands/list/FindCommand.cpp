//
//  FindCommand.cpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#include "FindCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"

bool jasl::FindCommand::m_registered = 
registerCommand<jasl::FindCommand>();

namespace jasl
{
    FindCommand::FindCommand(Function &func_,
                             SharedCacheStack const &sharedCache,
                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    , m_matches(sharedCache)
    {
    }

    FindCommand::~FindCommand() = default;

    std::vector<std::string> FindCommand::getCommandNames()
    {
        return {"find"};
    }

    bool FindCommand::execute() 
    {
        // Try and extract List 1
        List list1;
        if(!m_func.getValueA<decltype(list1)>(list1, m_sharedCache)) {
            std::string varName;
            if(m_func.getValueA<decltype(varName)>(varName, m_sharedCache)) {
                auto theList = m_sharedCache->getVar<List>(varName, Type::List);
                if(theList) {
                    list1 = *theList;
                }
            } else {
                setLastErrorMessage("find: error");
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
                setLastErrorMessage("find: error");
                return false;
            }
        }

        std::string resultName;
        if(!m_func.getValueC<std::string>(resultName, m_sharedCache)) {
            setLastErrorMessage("find: error");
            return false;
        }

        List result;
        for(auto const & element : list1) {

            // Try string
            std::string str;
            auto const strSuccess = VarExtractor::tryAnyCast(str, element);
            if(strSuccess) {
                List listWithString;
                listWithString.push_back(str);
                if (m_matches.matches(listWithString, list2)) {
                    result.push_back(element);
                }
            }
            // Try list
            else {
                List listEx;
                auto const listSuccess = VarExtractor::tryAnyCast(listEx, element);
                if(listSuccess) {
                    if (m_matches.matches(listEx, list2)) {
                        result.push_back(element);
                    }
                }
            }

        }

        m_sharedCache->setVar(resultName, result, Type::List);
        return true;
    }
}
