//
//  TypeCommand.cpp
//  jasl
//
//  Created by Ben Jones on 15/11/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "TypeCommand.hpp"
#include "../VarExtractor.hpp"
#include <algorithm>
#include <vector>
#include <sstream>

namespace {
    /// Convert a type enum value to a string representation
    std::string getType(jasl::Type const type) 
    {
        switch(type) {
            case jasl::Type::Int: return "int";
            case jasl::Type::Bool: return "bool";
            case jasl::Type::Real: return "real";
            case jasl::Type::Byte: return "byte";
            case jasl::Type::List: return "list";
            case jasl::Type::IntArray: return "array:int";
            case jasl::Type::RealArray: return "array:real";
            case jasl::Type::ByteArray: return "array:byte";
            default: return "string";
        }
    }
}

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

        m_sharedCache->setVar(key, getType(*type), Type::String);
        return true;
    }
}
