//
//  ListCommand.cpp
//  jasl
//
//  Created by Ben Jones on 08/11/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "ListCommand.hpp"
#include "../VarExtractor.hpp"

namespace jasl
{

    ListCommand::ListCommand(Function &func_,
                             SharedVarCache const &sharedCache,
                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {

    }

    bool ListCommand::execute() 
    {
        // now try and extract the actual words
        ValueArray list;
        if(!m_func.getValueA<decltype(list)>(list, m_sharedCache)) {
            std::string varName;
            if(m_func.getValueA<decltype(varName)>(varName, m_sharedCache)) {
                auto theList = m_sharedCache->getList(varName);
                if(theList) {
                    list = *theList;
                }
            } else {
                setLastErrorMessage("list: couldn't understand list");
                return false;
            }
        }

        std::string listName;
        if(!m_func.getValueB<std::string>(listName, m_sharedCache)) {
            setLastErrorMessage("list: couldn't parse name");
            return false;
        }

        m_sharedCache->setVar(listName, list, Type::ValueArray);
        return true;
    }
}