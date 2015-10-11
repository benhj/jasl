//
//  TypeCommand.cpp
//  jasl
//
//  Created by Ben Jones on 03/10/15
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
    class TypeCommand : public Command
    {
    public:
        TypeCommand(Function &func_,
                    SharedVarCache const &sharedCache = SharedVarCache(),
                    OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {

        }

        bool execute() override
        {
            // now try and extract the var symbol
            std::string symbol;
            if(!m_func.getValueA<decltype(symbol)>(symbol)) {
                setLastErrorMessage("type: couldn't determine argument");
                return false;
            }

            // determine var key
            std::string key;
            if(!m_func.getValueB<decltype(key)>(key)) {
                setLastErrorMessage("type: couldn't determine var key");
                return false;
            }

            // determine type
            auto type = VarCache::getType(symbol);
            if(!type) {
                setLastErrorMessage("type: couldn't determine type");
                return false;
            }

            std::string theType;
            if(*type == Type::Int) {
                theType = "integer";
            } else if(*type == Type::Bool) {
                theType = "boolean";
            } else if(*type == Type::Double) {
                theType = "decimal";
            } else if(*type == Type::ValueArray) {
                theType = "list";
            }  else {
                theType = "string";
            }

            VarCache::setString(key, theType);
            return true;
        }
        
    };

}