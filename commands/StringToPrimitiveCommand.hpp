//
//  StringToPrimitiveCommand.cpp
//  jasl
//
//  Created by Ben Jones on 13/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../LiteralString.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <sstream>
#include <cstdint>

namespace jasl
{
    class StringToPrimitiveCommand : public Command
    {
    public:
        StringToPrimitiveCommand(Function &func_,
                                 OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {
        }

        bool execute() override
        {
            std::string varName;
            if(!m_func.getValueB<std::string>(varName)) {
                m_errorMessage = "string_to_integer: couldn't parse integer name";
                appendToOutput(m_errorMessage);
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
            if(m_func.getValueA<LiteralString>(literalString)) {
                try {
                    VarCache::intCache[varName] = boost::lexical_cast<int64_t>(literalString.literal);
                    return true;
                } catch( boost::bad_lexical_cast const& ) {
                    m_errorMessage = "string_to_integer: couldn't parse string literal.";
                    appendToOutput(m_errorMessage);
                    return false;
                }
            }
            return false;
        }

        bool trySymbolExtraction(std::string const &varName)
        {
            // Now try extracting a symbol
            std::string symbol;
            if(m_func.getValueA<std::string>(symbol)) {
                auto result = VarExtractor::searchString(symbol);
                if(result) {

                    try {
                        VarCache::intCache[varName] = boost::lexical_cast<int64_t>(*result);
                        return true;
                    } catch( boost::bad_lexical_cast const& ) {
                        m_errorMessage = "string_to_integer: couldn't parse string literal.";
                        appendToOutput(m_errorMessage);
                        return false;
                    }
                }
            }
            return false;
        }

        
    };

}