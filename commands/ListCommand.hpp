//
//  ListCommand.cpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../VarExtractor.hpp"
#include <algorithm>
#include <vector>
#include <sstream>

namespace jasl
{
    class ListCommand : public Command
    {
    public:
        ListCommand(Function &func_,
                    SharedVarCache const &sharedCache = SharedVarCache(),
                    OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {

        }

        bool execute() override
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

            // add list to list cache
            if(GlobalCache::getList(listName)) {
                GlobalCache::setList(listName, list);
            }
            m_sharedCache->setList(listName, list);
            return true;
        }
        
    };

}