//
//  StringLengthCommand.cpp
//  jasl
//
//  Created by Ben Jones on 13/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../LiteralString.hpp"
#include "../VarExtractor.hpp"
#include <algorithm>
#include <sstream>

namespace jasl
{
    class StringLengthCommand : public Command
    {
    public:
        StringLengthCommand(Function &func_,
                            SharedVarCache const &sharedCache = SharedVarCache(),
                            OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {

        }

        bool execute() override
        {
            std::string varName;
            if(!m_func.getValueB<std::string>(varName, m_sharedCache)) {
                setLastErrorMessage("string_length: couldn't parse");
                return false;
            }

            if(tryLiteralExtraction(varName)) { return true; }
            if(trySymbolExtraction(varName)) { return true; }

            return false;
        }
    private:
        bool tryLiteralExtraction(std::string const &varName) 
        {
            LiteralString literalString;
            if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
                m_sharedCache->setInt(varName, literalString.literal.length());
                return true;
            }
            return false;
        }

        bool trySymbolExtraction(std::string const &varName)
        {
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
                auto result = m_sharedCache->getString(symbol);
                if(result) {
                    m_sharedCache->setInt(varName, result->length());
                    return true;
                }
            }
            return false;
        }

        
    };

}