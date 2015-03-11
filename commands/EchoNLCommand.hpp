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
            // First try extracting a string literal
            LiteralString literalString;
            if(m_func.getValueA<LiteralString>(literalString)) {
                appendToOutputWithNewLine(literalString.literal);
                return true;
            }

            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol)) {
                {
                    auto result = VarExtractor::searchInt(symbol);
                    if(result) {
                        std::cout<<"YES!!!!"<<std::endl;
                        appendToOutputWithNewLine(*result);
                        return true;
                    }
                }
                {
                    auto result = VarExtractor::searchDouble(symbol);
                    if(result) {
                        appendToOutputWithNewLine(*result);
                        return true;
                    }
                }
                {
                    auto result = VarExtractor::searchBool(symbol);
                    if(result) {
                        appendToOutputWithNewLine(*result);
                        return true;
                    }
                }
                {
                    auto result = VarExtractor::searchString(symbol);
                    if(result) {
                        appendToOutputWithNewLine(*result);
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