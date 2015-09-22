//
//  EchoNLCommand.hpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../LiteralString.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"

namespace jasl
{
    class EchoNLCommand : public Command
    {
    public:
        EchoNLCommand(Function &func_,
                      OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {

        }

        bool execute() override
        {
            if(tryLiteralExtraction()) { return true; }
            if(trySymbolExtraction()) { return true; }
            if(tryNumericExtraction()) { return true; }
            setLastErrorMessage("echo_nl: couldn't parse");
            return false;
        }

    private:
        bool tryLiteralExtraction() 
        {
            LiteralString literalString;
            if(m_func.getValueA<LiteralString>(literalString)) {
                appendToOutputWithNewLine(literalString.literal);
                return true;
            }
            return false;
        }

        bool trySymbolExtraction()
        {
                        // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol)) {
                {
                    auto result = VarCache::getInt(symbol);
                    if(result) {
                        appendToOutputWithNewLine(*result);
                        return true;
                    }
                }
                {
                    auto result = VarCache::getDouble(symbol);
                    if(result) {
                        appendToOutputWithNewLine(*result);
                        return true;
                    }
                }
                {
                    auto result = VarCache::getBool(symbol);
                    if(result) {
                        appendToOutputWithNewLine(*result);
                        return true;
                    }
                }
                {
                    auto result = VarCache::getString(symbol);
                    if(result) {
                        appendToOutputWithNewLine(*result);
                        return true;
                    }
                }
                return true;
            }

            return false;
        }

        bool tryNumericExtraction()
        {
            {
                auto result = VarExtractor::tryToGetADouble(m_func.paramA);
                if(result) {
                    appendToOutputWithNewLine(*result);
                    return true;
                }
            }

            {
                auto result = VarExtractor::trySingleBoolExtraction(m_func.paramA);
                if(result) {
                    appendToOutputWithNewLine(*result);
                    return true;
                }
            }

            return false;

        }
    };
}