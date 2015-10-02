//
//  EchoCommand.hpp
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
    class EchoCommand : public Command
    {
    public:
        EchoCommand(Function &func_,
                    OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {

        }

        bool execute() override
        {
            if(tryLiteralExtraction()) { return true; }
            if(trySymbolExtraction()) { return true; }
            if(tryNumericExtraction()) { return true; }
            setLastErrorMessage("echo: couldn't parse");
            return false;
        }

    private:
        bool tryLiteralExtraction() 
        {
            LiteralString literalString;
            if(m_func.getValueA<LiteralString>(literalString)) {
                appendToOutput(literalString.literal);
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
                        appendToOutput(*result);
                        return true;
                    }
                }
                {
                    auto result = VarCache::getDouble(symbol);
                    if(result) {
                        appendToOutput(*result);
                        return true;
                    }
                }
                {
                    auto result = VarCache::getBool(symbol);
                    if(result) {
                        appendToOutput(*result);
                        return true;
                    }
                }
                {
                    auto result = VarCache::getString(symbol);
                    if(result) {
                        appendToOutput(*result);
                        return true;
                    }
                }
                {
                    auto result = VarCache::getList(symbol);
                    if(result) {
                        processListElement(*result);
                        appendToOutput("\n");
                        return true;
                    }
                }
                return true;
            }

            return false;
        }

        void processListElement(ValueArray const &valueArray)
        {
            // Print out tokens, one after another
            for(auto const & it : valueArray) {
                // First try pulling a string out
                {
                    std::string tok;
                    if(VarExtractor::tryAnyCast(tok, it)) {
                        appendToOutput(tok);
                        appendToOutput(" ");
                    }
                }
                // Second, try pulling ValueArray out (nb, a nested list)
                {
                    ValueArray tok;
                    if(VarExtractor::tryAnyCast(tok, it)) {
                        processListElement(tok);
                    }
                }
            }
        }

        bool tryNumericExtraction()
        {

            {
                auto result = VarExtractor::tryToGetADouble(m_func.paramA);
                if(result) {
                    appendToOutput(*result);
                    return true;
                }
            }

            {
                auto result = VarExtractor::trySingleBoolExtraction(m_func.paramA);
                if(result) {
                    appendToOutput(*result);
                    return true;
                }
            }

            return false;

        }

    };
}