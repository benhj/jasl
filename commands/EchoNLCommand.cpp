//
//  EchoNLCommand.cpp
//  jasl
//
//  Created by Ben Jones 25/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "EchoNLCommand.hpp"
#include "../LiteralString.hpp"
#include "../VarExtractor.hpp"
#include <boost/algorithm/string.hpp>

namespace jasl
{
    EchoNLCommand::EchoNLCommand(Function &func_,
                                 SharedVarCache const &sharedCache,
                                 OptionalOutputStream const &output)
    : Command(func_, std::move(sharedCache), std::move(output))
    {

    }

    bool EchoNLCommand::execute()
    {
        if(tryLiteralExtraction()) { return true; }
        if(trySymbolExtraction()) { return true; }
        if(tryNumericExtraction()) { return true; }
        setLastErrorMessage("echo_nl: couldn't parse");
        return false;
    }

    bool EchoNLCommand::tryLiteralExtraction() 
    {
        LiteralString literalString;
        if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
            appendToOutputWithNewLine(literalString.literal);
            return true;
        }
        return false;
    }

    bool EchoNLCommand::trySymbolExtraction()
    {
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
            {
                auto result = m_sharedCache->getInt(symbol);
                if(result) {
                    appendToOutputWithNewLine(*result);
                    return true;
                }
            }
            {
                auto result = m_sharedCache->getDouble(symbol);
                if(result) {
                    appendToOutputWithNewLine(*result);
                    return true;
                }
            }
            {
                auto result = m_sharedCache->getBool(symbol);
                if(result) {
                    appendToOutputWithNewLine(*result);
                    return true;
                }
            }
            {
                auto result = m_sharedCache->getString(symbol);
                if(result) {
                    appendToOutputWithNewLine(*result);
                    return true;
                }
            }
            {
                auto result = m_sharedCache->getList(symbol);
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

    void EchoNLCommand::processListElement(ValueArray const &valueArray, std::string &output)
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

    bool EchoNLCommand::tryNumericExtraction()
    {
        {
            auto result = VarExtractor::tryToGetADouble(m_func.paramA, m_sharedCache);
            if(result) {
                appendToOutputWithNewLine(*result);
                return true;
            }
        }

        {
            auto result = VarExtractor::trySingleBoolExtraction(m_func.paramA, m_sharedCache);
            if(result) {
                appendToOutputWithNewLine(*result);
                return true;
            }
        }

        return false;
    }
}