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
            // First try extracting a string literal
            LiteralString literalString;
            if(m_func.getValueA<LiteralString>(literalString)) {
                appendToOutput(literalString.literal);
                return true;
            }

            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol)) {
                {
                    auto result = VarExtractor::searchInt(symbol);
                    if(result) {
                        appendToOutput(*result);
                        return true;
                    }
                }
                {
                    auto result = VarExtractor::searchDouble(symbol);
                    if(result) {
                        appendToOutput(*result);
                        return true;
                    }
                }
                {
                    auto result = VarExtractor::searchBool(symbol);
                    if(result) {
                        appendToOutput(*result);
                        return true;
                    }
                }
                {
                    auto result = VarExtractor::searchString(symbol);
                    if(result) {
                        appendToOutput(*result);
                        return true;
                    }
                }
                return true;
            }
            m_errorMessage = "echo: couldn't parse";
            appendToOutput(m_errorMessage);
            return false;
        }
    };
}