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
    : Command(func_, sharedCache, output)
    {

    }

    bool EchoNLCommand::execute()
    {
        if(tryLiteralExtraction()) { return true; }
        if(trySymbolExtraction()) { return true; }
        if(tryNumericExtraction()) { return true; }
        setLastErrorMessage("prn: couldn't parse");
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
                int64_t value;
                if(m_sharedCache->getInt_(symbol, value)) {
                    appendToOutputWithNewLine(value);
                    return true;
                }
            }
            {
                double value;
                if(m_sharedCache->getDouble_(symbol, value)) {
                    appendToOutputWithNewLine(value);
                    return true;
                }
            }
            {
                bool value;
                if(m_sharedCache->getBool_(symbol, value)) {
                    appendToOutputWithNewLine(value);
                    return true;
                }
            }
            {
                std::string value;
                if(m_sharedCache->getString_(symbol, value)) {
                    appendToOutputWithNewLine(value);
                    return true;
                }
            }
            {
                ValueArray value;
                auto result = m_sharedCache->getList_(symbol, value);
                if(result) {
                    std::string output;
                    processListElement(value, output);
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
            double value;
            if(VarExtractor::tryToGetADouble(m_func.paramA, value, m_sharedCache)) {
                appendToOutputWithNewLine(value);
                return true;
            }
        }

        {
            bool value;
            if(VarExtractor::trySingleBoolExtraction(m_func.paramA, value, m_sharedCache)) {
                appendToOutputWithNewLine(value);
                return true;
            }
        }

        return false;
    }
}