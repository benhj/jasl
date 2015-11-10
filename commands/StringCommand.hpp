//
//  StringCommand.cpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "ListToStringCommand.hpp"
#include "../VarExtractor.hpp"
#include <sstream>

namespace jasl
{
    class StringCommand : public Command
    {
    public:
        StringCommand(Function &func_,
                      SharedVarCache const &sharedCache = SharedVarCache(),
                      OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, std::move(sharedCache), std::move(output))
        {

        }

        bool execute() override
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
            setLastErrorMessage("echo: couldn't parse");
            return false;
        }

    private:
        bool tryLiteralExtraction(std::string const &key) 
        {
            LiteralString literalString;
            if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
                m_sharedCache->setString(key, literalString.literal);
                return true;
            }
            return false;
        }

        bool trySymbolExtraction(std::string const &key)
        {
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
                {
                    int64_t value;
                    if( m_sharedCache->getInt_(symbol, value)) {

                        m_sharedCache->setString(key, std::to_string(value));
                        
                        return true;
                    }
                }
                {
                    auto result = m_sharedCache->getDouble(symbol);
                    if(result) {

                        m_sharedCache->setString(key, std::to_string(*result));
                        
                        return true;
                    }
                }
                {
                    auto result = m_sharedCache->getBool(symbol);
                    if(result) {

                        m_sharedCache->setString(key, std::to_string(*result));
                        
                        return true;
                    }
                }
                {
                    auto result = m_sharedCache->getString(symbol);
                    if(result) {

                        m_sharedCache->setString(key, *result);
                        
                        return true;
                    }
                }
            }

            return false;
        }

        bool tryNumericExtraction(std::string const &key)
        {

            {
                auto result = VarExtractor::trySingleIntExtractionNoMath(m_func.paramA, m_sharedCache);
                if(result) {
                    m_sharedCache->setString(key, std::to_string(*result));
                    return true;
                }
            }

            {
                auto result = VarExtractor::tryToGetADouble(m_func.paramA, m_sharedCache);
                if(result) {
                    std::ostringstream ss;
                    ss << *result;
                    m_sharedCache->setString(key, ss.str());                    
                    return true;
                }
            }

            {
                auto result = VarExtractor::trySingleBoolExtraction(m_func.paramA, m_sharedCache);
                if(result) {
                    m_sharedCache->setString(key, std::to_string(*result));
                    return true;
                }
            }

            return false;

        }

        bool tryListExtraction()
        {
            return ListToStringCommand(m_func, m_sharedCache, m_outputStream).execute();
        }

    };

}