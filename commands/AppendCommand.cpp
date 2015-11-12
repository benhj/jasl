//
//  AppendCommand.cpp
//  jasl
//
//  Created by Ben Jones on 25/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "AppendCommand.hpp"
#include "../VarExtractor.hpp"
#include <sstream>

namespace jasl
{

    AppendCommand::AppendCommand(Function &func_,
                                 SharedVarCache const &sharedCache,
                                 OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {

    }

    bool AppendCommand::execute()
    {

        std::string key;
        if(!m_func.getValueC<std::string>(key, m_sharedCache)) {
            return false;
        }

        if(tryLiteralExtraction(key)) { return true; }
        if(trySymbolExtraction(key)) { return true; }
        if(tryNumericExtraction(key)) { return true; }
        setLastErrorMessage("append: couldn't parse");
        return false;
    }

    OptionalString AppendCommand::getStringBeingAppendedTo()
    {
        std::string stringBeingAppendedTo;
        if(!m_func.getValueA<std::string>(stringBeingAppendedTo, m_sharedCache)) {
            LiteralString literal;
            if(!m_func.getValueA<LiteralString>(literal, m_sharedCache)) {
                return OptionalString();
            }
            return OptionalString(literal.literal);
        }
        return m_sharedCache->getString(stringBeingAppendedTo);
    }

    bool AppendCommand::tryLiteralExtraction(std::string const &key) 
    {
        auto stringBeingAppendedTo(getStringBeingAppendedTo());
        if(!stringBeingAppendedTo) { return false; }
        LiteralString literalString;
        if(m_func.getValueB<LiteralString>(literalString, m_sharedCache)) {
            m_sharedCache->setString(key, (*stringBeingAppendedTo).append(literalString.literal));
            return true;
        }
        return false;
    }

    bool AppendCommand::trySymbolExtraction(std::string const &key)
    {
        auto stringBeingAppendedTo(getStringBeingAppendedTo());
        if(!stringBeingAppendedTo) { return false; }

        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueB<std::string>(symbol, m_sharedCache)) {
            {
                int64_t value;
                
                if(m_sharedCache->getInt_(symbol, value)){
                    m_sharedCache->setString(key, (*stringBeingAppendedTo).append(std::to_string(value)));
                    return true;
                }
            }
            {
                double value;
                if(m_sharedCache->getDouble_(symbol, value)) {
                    m_sharedCache->setString(key, (*stringBeingAppendedTo).append(std::to_string(value)));
                    return true;
                }
            }
            {
                bool value;
                if(m_sharedCache->getBool_(symbol, value)) {
                    m_sharedCache->setString(key, (*stringBeingAppendedTo).append(std::to_string(value)));
                    return true;
                }
            }
            {
                std::string value;
                if(m_sharedCache->getString_(symbol, value)) {
                    m_sharedCache->setString(key, (*stringBeingAppendedTo).append(value));
                    return true;
                }
            }
            return true;
        }

        return false;
    }

    bool AppendCommand::tryNumericExtraction(std::string const &key)
    {
        auto stringBeingAppendedTo(getStringBeingAppendedTo());
        if(!stringBeingAppendedTo) { return false; }

        {
            int64_t value;
            if(VarExtractor::trySingleIntExtractionNoMath(m_func.paramB, value, m_sharedCache)) {
                m_sharedCache->setString(key, (*stringBeingAppendedTo).append(std::to_string(value)));
                return true;
            }
        }

        {
            double value;
            if(VarExtractor::tryToGetADouble(m_func.paramB, value, m_sharedCache)) {
                std::ostringstream ss;
                ss << value;
                m_sharedCache->setString(key, (*stringBeingAppendedTo).append(ss.str()));
                return true;
            }
        }

        {
            bool value;
            if(VarExtractor::trySingleBoolExtraction(m_func.paramB, value, m_sharedCache)) {
                m_sharedCache->setString(key, (*stringBeingAppendedTo).append(std::to_string(value)));
                return true;
            }
        }

        return false;
    }
}