//
//  LengthCommand.cpp
//  jasl
//
//  Created by Ben Jones on 15/11/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "LengthCommand.hpp"
#include "core/LiteralString.hpp"
#include "core/RegisterCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <algorithm>
#include <sstream>

namespace jasl
{
    LengthCommand::LengthCommand(Function &func_,
                                 SharedCacheStack const &sharedCache,
                                 OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    std::vector<std::string> LengthCommand::getCommandNames()
    {
        return {"length"};
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
                auto result = m_sharedCache->getVar<std::string>(symbol, Type::String);
                if(result) {
                    m_sharedCache->setVar(varName, (int64_t)result->length(), Type::Int);
                    return true;
                }
            }

            // try int array
            {
                auto array = m_sharedCache->getVar<IntArray>(symbol, Type::IntArray);
                if(array) {
                    m_sharedCache->setVar(varName, (int64_t)array->size(), Type::Int);
                    return true;
                }
            }

            // try real array
            {
                auto array = m_sharedCache->getVar<RealArray>(symbol, Type::RealArray);
                if(array) {
                    m_sharedCache->setVar(varName, (int64_t)array->size(), Type::Int);
                    return true;
                }
            }

            // try byte array
            {
                auto array = m_sharedCache->getVar<ByteArray>(symbol, Type::ByteArray);
                if(array) {
                    m_sharedCache->setVar(varName, (int64_t)array->size(), Type::Int);
                    return true;
                }
            }

            // try string array
            {
                auto array = m_sharedCache->getVar<StringArray>(symbol, Type::StringArray);
                if(array) {
                    m_sharedCache->setVar(varName, (int64_t)array->size(), Type::Int);
                    return true;
                }
            }
        }
        return false;
    }
}
