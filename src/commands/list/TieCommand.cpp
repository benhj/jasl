//
//  TieCommand.cpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#include "TieCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <vector>

bool jasl::TieCommand::m_registered = 
registerCommand<jasl::TieCommand>();

namespace jasl
{
    TieCommand::TieCommand(Function &func_,
                                   SharedCacheStack const &sharedCache,
                                   OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    TieCommand::~TieCommand() = default;

    std::vector<std::string> TieCommand::getCommandNames()
    {
        return {"tie"};
    }

    bool TieCommand::execute() 
    {

        ValueArray paramArray;
        if(!m_func.getValueB<ValueArray>(paramArray, m_sharedCache)) {
            setLastErrorMessage("tie: error");
            return false;
        }

        std::vector<List> lists;
        for(auto const & v : paramArray) {

            // Try symbol
            std::string symbol;
            if(VarExtractor::tryAnyCast(symbol, v)) {
                auto const list = m_sharedCache->getVar<List>(symbol, Type::List);
                if(list) {
                    lists.push_back(*list);
                    continue;
                }
            }

            // Try raw list
            List list;
            if(VarExtractor::tryAnyCast(list, v)) {
                lists.push_back(list);
                continue;
            }
        }

        if(lists.empty()) {
            setLastErrorMessage("tie: no lists");
            return false;
        }

        auto current = std::begin(lists);
        auto length = current->size();
        ++current;

        for(; current != std::end(lists); ++current) {
            auto const listLength = current->size();
            if(length != listLength) {
                setLastErrorMessage("tie: lists different lengths");
                return false;
            }
        }

        //m_sharedCache->setVar(resultName, matchesResult, Type::Bool);
        return true;
    }
}
