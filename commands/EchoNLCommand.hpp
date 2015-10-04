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

#include <boost/algorithm/string.hpp>

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
                {
                    auto result = VarCache::getList(symbol);
                    if(result) {
                        std::string output;
                        processListElement(*result, output);
                        ::boost::algorithm::trim_right(output);
                        appendToOutputWithNewLine(output);
                        return true;
                    }
                }
                return true;
            }

            return false;
        }

        void processListElement(ValueArray const &valueArray, std::string &output)
        {
            output.append("[");
            // Print out tokens, one after another
            size_t count = 0;
            for(auto const & it : valueArray) {
                // First try pulling a string out
                {
                    std::string tok;
                    if(VarExtractor::tryAnyCast(tok, it)) {
                        output.append(tok);
                        if(count < valueArray.size() - 1) {
                            output.append(" ");
                        }
                    }
                }
                // Second, try pulling ValueArray out (nb, a nested list)
                {
                    ValueArray tok;
                    if(VarExtractor::tryAnyCast(tok, it)) {
                        processListElement(tok, output);
                    }
                }
                ++count;
            }
            output.append("] ");
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