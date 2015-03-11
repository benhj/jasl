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
#include "../VarCache.hpp"
#include <algorithm>
#include <vector>
#include <sstream>

namespace jasl
{
    class ListCommand : public Command
    {
    public:
        ListCommand(Function &func_,
                    OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {

        }

        bool execute() override
        {
            std::cout<<"HERE!!!"<<std::endl;
            std::string listName;
            if(!m_func.getValueA<std::string>(listName)) {
                m_errorMessage = "list: couldn't parse name";
                appendToOutput(m_errorMessage);
                return false;
            }

            std::cout<<"break!!!"<<std::endl;

            // now try and extract the actual words
            std::vector<std::string> list;
            if(!m_func.getValueB<decltype(list)>(list)) {
                m_errorMessage = "list: couldn't parse word list";
                return false;
            }

            std::cout<<"done!!!"<<std::endl;

            // add list to list cache
            VarCache::listCache[listName] = list;
            return true;

        }
        
    };

}