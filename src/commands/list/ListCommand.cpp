//
//  ListCommand.cpp
//  jasl
//
//  Created by Ben Jones on 08/11/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "ListCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"

bool jasl::ListCommand::m_registered = 
registerCommand<jasl::ListCommand>();

namespace jasl
{

    ListCommand::ListCommand(Function &func_,
                             SharedCacheStack const &sharedCache,
                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    ListCommand::~ListCommand() = default;

    std::vector<std::string> ListCommand::getCommandNames()
    {
        return {"list"};
    }

    bool ListCommand::execute() 
    {
        // now try and extract the actual words
        List list;
        if(!m_func.getValueA<decltype(list)>(list, m_sharedCache)) {
            std::string varName;
            if(m_func.getValueA<decltype(varName)>(varName, m_sharedCache)) {
                auto theList = m_sharedCache->getVar<List>(varName, Type::List);
                if(theList) {
                    list = *theList;
                }
            } else {
                setLastErrorMessage("list: couldn't understand list");
                return false;
            }
        }

        // Loop through list, and check if we have any ^element
        List finalList;
        for(auto const & el : list) {
            std::string element;
            if(VarExtractor::tryAnyCast(element, el)) {
                if(element.length() > 1 && element[0] == '^') {
                    std::string const varName(std::begin(element) + 1, std::end(element));

                    // Try and get a string from ^var
                    auto const val = m_sharedCache->getVar<std::string>(varName, Type::String);
                    if(val) {
                        finalList.push_back(*val);
                    } 
                    // Try and get a list from ^var
                    else {
                        auto const listVal = m_sharedCache->getVar<std::string>(varName, Type::List);
                        if(listVal) {
                            finalList.push_back(*listVal);
                        }
                    }
                } else {
                    finalList.push_back(el);
                }
            } else {
                finalList.push_back(el);
            }
        }

        std::string listName;
        if(!m_func.getValueB<std::string>(listName, m_sharedCache)) {
            setLastErrorMessage("list: couldn't parse name");
            return false;
        }

        m_sharedCache->setVar(listName, finalList, Type::List);
        return true;
    }
}
