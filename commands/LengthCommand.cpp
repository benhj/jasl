//
//  LengthCommand.cpp
//  jasl
//
//  Created by Ben Jones on 15/11/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "LengthCommand.hpp"
#include "../LiteralString.hpp"
#include "../VarExtractor.hpp"
#include <algorithm>
#include <sstream>

namespace jasl
{
    LengthCommand::LengthCommand(Function &func_,
                                 SharedVarCache const &sharedCache,
                                 OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool LengthCommand::execute() 
    {
        std::string varName;
        if(!m_func.getValueB<std::string>(varName, m_sharedCache)) {
            setLastErrorMessage("length: couldn't parse");
            return false;
        }

        if(tryLiteralExtraction(varName)) { return true; }
        if(trySymbolExtraction(varName)) { return true; }

        return false;
    }

    bool LengthCommand::tryLiteralExtraction(std::string const &varName)
    {
        LiteralString literalString;
        if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
            m_sharedCache->setVar(varName, (int64_t)literalString.literal.length(), Type::Int);
            return true;
        }
        return false;
    }

    bool LengthCommand::trySymbolExtraction(std::string const &varName)
    {
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {

            // try string
            {
                auto result = m_sharedCache->getString(symbol);
                if(result) {
                    m_sharedCache->setVar(varName, (int64_t)result->length(), Type::Int);
                    return true;
                }
            }

            // try int array
            {
                auto array = m_sharedCache->getIntArray(symbol);
                if(array) {
                    m_sharedCache->setVar(varName, (int64_t)array->size(), Type::Int);
                    return true;
                }
            }

            // try real array
            {
                auto array = m_sharedCache->getDoubleArray(symbol);
                if(array) {
                    m_sharedCache->setVar(varName, (int64_t)array->size(), Type::Int);
                    return true;
                }
            }

            // try byte array
            {
                auto array = m_sharedCache->getByteArray(symbol);
                if(array) {
                    m_sharedCache->setVar(varName, (int64_t)array->size(), Type::Int);
                    return true;
                }
            }
        }
        return false;
    }
}
