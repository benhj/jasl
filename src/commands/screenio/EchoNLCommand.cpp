//
//  EchoNLCommand.cpp
//  jasl
//
//  Created by Ben Jones 25/10/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "EchoNLCommand.hpp"
#include "core/LiteralString.hpp"
#include "core/RegisterCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <boost/algorithm/string.hpp>

bool jasl::EchoNLCommand::m_registered = registerCommand<jasl::EchoNLCommand>();

namespace jasl
{
    EchoNLCommand::EchoNLCommand(Function &func_,
                                 SharedCacheStack const &sharedCache,
                                 OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    EchoNLCommand::~EchoNLCommand() = default;

    std::vector<std::string> EchoNLCommand::getCommandNames()
    {
        return {"prn"};
    }

    bool EchoNLCommand::execute()
    {
        if(tryLiteralExtraction()) { return true; }
        if(tryListExtraction()) { return true; }
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

    bool EchoNLCommand::tryListExtraction()
    {
        List list;
        if(m_func.getValueA<List>(list, m_sharedCache)) {
            std::string output;
            processListElement(list, output);
            ::boost::algorithm::trim_right(output);
            appendToOutputWithNewLine(output);
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
                if(m_sharedCache->getVar_(symbol, value, Type::Int)) {
                    appendToOutputWithNewLine(value);
                    return true;
                }
            }
            {
                uint8_t value;
                if(m_sharedCache->getVar_(symbol, value, Type::Byte)) {
                    appendToOutputWithNewLine(value);
                    return true;
                }
            }
            {
                double value;
                if(m_sharedCache->getVar_(symbol, value, Type::Real)) {
                    appendToOutputWithNewLine(value);
                    return true;
                }
            }
            {
                bool value;
                if(m_sharedCache->getVar_(symbol, value, Type::Bool)) {
                    appendToOutputWithNewLine(value);
                    return true;
                }
            }
            {
                std::string value;
                if(m_sharedCache->getVar_(symbol, value, Type::String)) {
                    appendToOutputWithNewLine(value);
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
                    appendToOutputWithNewLine(output);
                    return true;
                }
            }
            return true;
        }

        return false;
    }

    void EchoNLCommand::processListElement(List const &valueArray,
                                           std::string &output,
                                           int const depth,
                                           bool const lastToken)
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
                    processListElement(tok, output, depth + 1, 
                                       (count == valueArray.size() - 1));
                }
            }
            ++count;
        }
        if(depth > 0 && lastToken) {
            output.append("]");
        } else {
            output.append("] ");
        }
    }

    bool EchoNLCommand::tryNumericExtraction()
    {
        {
            double value;
            if(VarExtractor::tryToGetAReal(m_func.paramA, value, m_sharedCache)) {
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
