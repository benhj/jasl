//
//  AppendCommand.cpp
//  jasl
//
//  Created by Ben Jones on 11/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"
#include <sstream>

namespace jasl
{
    class AppendCommand : public Command
    {
    public:
        AppendCommand(Function &func_,
                      SharedVarCache const &sharedCache = SharedVarCache(),
                      OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {

        }

        bool execute() override
        {

            std::string key;
            if(!m_func.getValueC<std::string>(key)) {
                return false;
            }

            if(tryLiteralExtraction(key)) { return true; }
            if(trySymbolExtraction(key)) { return true; }
            if(tryNumericExtraction(key)) { return true; }
            setLastErrorMessage("append: couldn't parse");
            return false;
        }

    private:

        OptionalString getStringBeingAppendedTo()
        {
            std::string stringBeingAppendedTo;
            if(!m_func.getValueA<std::string>(stringBeingAppendedTo)) {
                LiteralString literal;
                if(!m_func.getValueA<LiteralString>(literal)) {
                    return OptionalString();
                }
                return OptionalString(literal.literal);
            }
            return VarCache::getString(stringBeingAppendedTo);
        }

        bool tryLiteralExtraction(std::string const &key) 
        {
            auto stringBeingAppendedTo(getStringBeingAppendedTo());
            if(!stringBeingAppendedTo) { return false; }
            LiteralString literalString;
            if(m_func.getValueB<LiteralString>(literalString)) {
                VarCache::setString(key, (*stringBeingAppendedTo).append(literalString.literal));
                return true;
            }
            return false;
        }

        bool trySymbolExtraction(std::string const &key)
        {
            auto stringBeingAppendedTo(getStringBeingAppendedTo());
            if(!stringBeingAppendedTo) { return false; }

            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueB<std::string>(symbol)) {
                {
                    auto result = VarCache::getInt(symbol);
                    if(result) {
                        VarCache::setString(key, (*stringBeingAppendedTo).append(std::to_string(*result)));
                        return true;
                    }
                }
                {
                    auto result = VarCache::getDouble(symbol);
                    if(result) {
                        VarCache::setString(key, (*stringBeingAppendedTo).append(std::to_string(*result)));
                        return true;
                    }
                }
                {
                    auto result = VarCache::getBool(symbol);
                    if(result) {
                        VarCache::setString(key, (*stringBeingAppendedTo).append(std::to_string(*result)));
                        return true;
                    }
                }
                {
                    auto result = VarCache::getString(symbol);
                    if(result) {
                        VarCache::setString(key, (*stringBeingAppendedTo).append(*result));
                        return true;
                    }
                }
                return true;
            }

            return false;
        }

        bool tryNumericExtraction(std::string const &key)
        {
            auto stringBeingAppendedTo(getStringBeingAppendedTo());
            if(!stringBeingAppendedTo) { return false; }

            {
                auto result = VarExtractor::trySingleIntExtractionNoMath(m_func.paramB);
                if(result) {
                    VarCache::setString(key, (*stringBeingAppendedTo).append(std::to_string(*result)));
                    return true;
                }
            }

            {
                auto result = VarExtractor::tryToGetADouble(m_func.paramB);
                if(result) {
                    std::ostringstream ss;
                    ss << *result;
                     VarCache::setString(key, (*stringBeingAppendedTo).append(ss.str()));
                    return true;
                }
            }

            {
                auto result = VarExtractor::trySingleBoolExtraction(m_func.paramB);
                if(result) {
                    VarCache::setString(key, (*stringBeingAppendedTo).append(std::to_string(*result)));
                    return true;
                }
            }

            return false;

        }

    };

}