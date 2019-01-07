//
//  EchoCommand.cpp
//  jasl
//
//  Created by Ben Jones 25/10/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "EchoCommand.hpp"
#include "core/LiteralString.hpp"
#include "core/RegisterCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <boost/algorithm/string.hpp>

/**
 * AUTO TYPE REGISTRAION
 */
bool jasl::EchoCommand::m_registered = registerCommand<jasl::EchoCommand>();

namespace jasl
{
    EchoCommand::EchoCommand(Function &func_,
                             SharedCacheStack const &sharedCache,
                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {

    }

    std::vector<std::string> EchoCommand::getCommandNames()
    {
        return {"pr","say"};
    }

    bool EchoCommand::execute()
    {
        if(tryLiteralExtraction()) { return true; }
        if(trySymbolExtraction()) { return true; }
        if(tryNumericExtraction()) { return true; }
        setLastErrorMessage("prn: couldn't parse");
        return false;
    }

    bool EchoCommand::tryLiteralExtraction() 
    {
        LiteralString literalString;
        if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
            appendToOutput(literalString.literal);
            return true;
        }
        return false;
    }

    bool EchoCommand::trySymbolExtraction()
    {
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
            {
                int64_t value;
                if(m_sharedCache->getVar_(symbol, value, Type::Int)) {
                    appendToOutput(value);
                    return true;
                }
            }
            {
                uint8_t value;
                if(m_sharedCache->getVar_(symbol, value, Type::Byte)) {
                    appendToOutput(value);
                    return true;
                }
            }
            {
                double value;
                if(m_sharedCache->getVar_(symbol, value, Type::Real)) {
                    appendToOutput(value);
                    return true;
                }
            }
            {
                bool value;
                if(m_sharedCache->getVar_(symbol, value, Type::Bool)) {
                    appendToOutput(value);
                    return true;
                }
            }
            {
                std::string value;
                if(m_sharedCache->getVar_(symbol, value, Type::String)) {
                    appendToOutput(value);
                    return true;
                }
            }
            {
                List value;
                auto result = m_sharedCache->getVar_(symbol, value, Type::List);
                if(result) {
                    std::string output;
                    processListElement(value, output);
                    ::boost::algorithm::trim_right(output);
                    appendToOutput(output);
                    return true;
                }
            }
            return true;
        }

        return false;
    }

    void EchoCommand::processListElement(List const &valueArray, std::string &output)
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
            // Second, try pulling List out (nb, a nested list)
            {
                List tok;
                if(VarExtractor::tryAnyCast(tok, it)) {
                    processListElement(tok, output);
                }
            }
            ++count;
        }
        output.append("] ");
    }

    bool EchoCommand::tryNumericExtraction()
    {
        {
            double value;
            if(VarExtractor::tryToGetAReal(m_func.paramA, value, m_sharedCache)) {
                appendToOutput(value);
                return true;
            }
        }

        {
            bool value;
            if(VarExtractor::trySingleBoolExtraction(m_func.paramA, value, m_sharedCache)) {
                appendToOutput(value);
                return true;
            }
        }

        return false;
    }
}