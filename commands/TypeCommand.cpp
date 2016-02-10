//
//  TypeCommand.cpp
//  jasl
//
//  Created by Ben Jones on 15/11/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "TypeCommand.hpp"
#include "../VarExtractor.hpp"
#include <algorithm>
#include <vector>
#include <sstream>

namespace jasl
{
    TypeCommand::TypeCommand(Function &func_,
                             SharedVarCache const &sharedCache,
                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool TypeCommand::execute()
    {
        // now try and extract the var symbol
        std::string symbol;
        if(!m_func.getValueA<decltype(symbol)>(symbol, m_sharedCache)) {
            setLastErrorMessage("type: couldn't determine argument");
            return false;
        }

        // determine var key
        std::string key;
        if(!m_func.getValueB<decltype(key)>(key, m_sharedCache)) {
            setLastErrorMessage("type: couldn't determine var key");
            return false;
        }

        // determine type
        auto type = m_sharedCache->getType(symbol);
        if(!type) {
            setLastErrorMessage("type: couldn't determine type");
            return false;
        }

        std::string theType;
        if(*type == Type::Int) {
            theType = "int";
        } else if(*type == Type::Bool) {
            theType = "bool";
        } else if(*type == Type::Double) {
            theType = "real";
        } else if(*type == Type::ValueArray) {
            theType = "list";
        }  else {
            theType = "string";
        }

        m_sharedCache->setString(key, theType);
        return true;
    }
}
