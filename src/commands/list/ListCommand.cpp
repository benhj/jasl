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
#include <algorithm>
#include <string>

bool jasl::ListCommand::m_registered = 
registerCommand<jasl::ListCommand>();

namespace {

    bool hasPunc(std::string & in) {
        auto const backChar = in.back();
        return backChar == '.' ||
               backChar == ',' ||
               backChar == '?' ||
               backChar == '!' ||
               backChar == ';' ||
               backChar == ':';
    }

    void format(std::string const & fname, std::string &in)
    {
        if(fname == "upper") {
            std::transform(std::begin(in),
                           std::end(in),
                           std::begin(in),
                           ::toupper);
        } else if(fname == "lower") {
            std::transform(std::begin(in),
                           std::end(in),
                           std::begin(in),
                           ::tolower);
        } 
        // Remove any trailing punctuation
        else if(fname == "rempunc") {
            if(hasPunc(in)) {
                in.pop_back();
            }
        }
    }
}

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
        return {"list", "lower", "upper", "rempunc"};
    }

    List ListCommand::processList(List const & list) const
    {
        List finalList;
        for(auto const & el : list) {
            std::string element;
            if(VarExtractor::tryAnyCast(element, el)) {
                if(element.length() > 1 && element[0] == '^') {

                    std::string varName;
                    auto start = 1;

                    // ^^insert
                    if(element.length() > 2 && element[1] == '^') {
                        ++start;   
                    }
                    varName = std::string(std::begin(element) + start, std::end(element));

                    // Try and get a string from ^var
                    auto const val = m_sharedCache->getVar<std::string>(varName, Type::String);
                    if(val) {
                        auto finalVal = *val;
                        format(m_func.name, finalVal);
                        finalList.push_back(finalVal);
                    } 
                    // Try and get a list from ^var or ^^var
                    else {
                        auto const listVal = m_sharedCache->getVar<List>(varName, Type::List);
                        if(listVal) {
                            if(start == 1) {
                                finalList.push_back(*listVal);
                            } else {
                                auto innerList = processList(*listVal);
                                finalList.insert(std::end(finalList), std::begin(innerList), std::end(innerList));
                            }
                        }
                    }
                } else {
                    auto finalVal = element;
                    format(m_func.name, finalVal);
                    finalList.push_back(finalVal);
                }
            } else {
                finalList.push_back(el);
            }
        }
        return finalList;
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
        List finalList = processList(list);

        std::string listName;
        if(!m_func.getValueB<std::string>(listName, m_sharedCache)) {
            setLastErrorMessage("list: couldn't parse name");
            return false;
        }

        m_sharedCache->setVar(listName, finalList, Type::List);
        return true;
    }
}
