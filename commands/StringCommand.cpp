//
//  StringCommand.cpp
//  jasl
//
//  Created by Ben Jones on 15/11/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "StringCommand.hpp"
#include "ListToStringCommand.hpp"
#include "../caching/VarExtractor.hpp"
#include <sstream>

namespace jasl
{
    StringCommand::StringCommand(Function &func_,
                                 SharedVarCache const &sharedCache,
                                 OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool StringCommand::execute() 
    {
        std::string stringName;
        if(!m_func.getValueB<std::string>(stringName, m_sharedCache)) {
            setLastErrorMessage("string: couldn't parse");
            return false;
        }

        if(tryLiteralExtraction(stringName)) { return true; }
        if(trySymbolExtraction(stringName)) { return true; }
        if(tryNumericExtraction(stringName)) { return true; }
        if(tryListExtraction()) { return true; }
        
        setLastErrorMessage("string: couldn't parse");
        return false;
    }

    bool StringCommand::tryLiteralExtraction(std::string const &key)
    {
        LiteralString literalString;
        if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
            m_sharedCache->setVar(key, literalString.literal, Type::String);
            return true;
        }
        return false;
    }

    bool StringCommand::trySymbolExtraction(std::string const &key)
    {
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
            {
                int64_t value;
                if( m_sharedCache->getVar_(symbol, value, Type::Int)) {
                    m_sharedCache->setVar(key, std::to_string(value), Type::String);
                    return true;
                }
            }
            {
                double value;
                if(m_sharedCache->getVar_(symbol, value, Type::Real)) {
                    m_sharedCache->setVar(key, std::to_string(value), Type::String);
                    return true;
                }
            }
            {
                bool value;
                if( m_sharedCache->getVar_(symbol, value, Type::Bool)) {
                    m_sharedCache->setVar(key, std::to_string(value), Type::String);
                    return true;
                }
            }
            {
                std::string value;
                if( m_sharedCache->getVar_(symbol, value, Type::String)) {
                    m_sharedCache->setVar(key, value, Type::String);
                    return true;
                }
            }
            {
                ByteArray value;
                if( m_sharedCache->getVar_(symbol, value, Type::ByteArray)) {

                    std::string str;
                    for (auto const & b : value) {
                        str += b;
                    }
                    m_sharedCache->setVar(key, str, Type::String);
                    return true;
                }
            }
        }

        return false;
    }

    bool StringCommand::tryNumericExtraction(std::string const &key)
    {

        {
            int64_t result;
            if(VarExtractor::trySingleIntExtractionNoMath(m_func.paramA, result, m_sharedCache)) {
                m_sharedCache->setVar(key, std::to_string(result), Type::String);
                return true;
            }
        }

        {
            double result;
            if(VarExtractor::tryToGetAReal(m_func.paramA, result, m_sharedCache)) {
                std::ostringstream ss;
                ss << result;
                m_sharedCache->setVar(key, ss.str(), Type::String);
                return true;
            }
        }

        {
            bool result;
            if(VarExtractor::trySingleBoolExtraction(m_func.paramA, result, m_sharedCache)) {
                m_sharedCache->setVar(key, std::to_string(result), Type::String);
                return true;
            }
        }

        return false;

    }

    bool StringCommand::tryListExtraction()
    {
        return ListToStringCommand(m_func, m_sharedCache, m_outputStream).execute();
    }
}
