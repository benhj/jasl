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
                setLastErrorMessage("string_to_: couldn't parse integer name");
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
                    // also handled off directy from NewPrimitiveSyntaxCommand so need
                    // to also check if function name is integer or decimal in respective branch
                    if(m_func.name == "string_to_integer" || m_func.name == "integer") {
                        VarCache::intCache[varName] = boost::lexical_cast<int64_t>(literalString.literal);
                        return true;
                    } else if(m_func.name == "string_to_decimal" || m_func.name == "decimal") {
                        VarCache::doubleCache[varName] = boost::lexical_cast<double>(literalString.literal);
                        return true;
                    }
                    return false;
                } catch( boost::bad_lexical_cast const& ) {
                    setLastErrorMessage("string_to_: couldn't parse string literal.");
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
                        // also handled off directy from NewPrimitiveSyntaxCommand so need
                        // to also check if function name is integer or decimal in respective branch
                        if(m_func.name == "string_to_integer" || m_func.name == "integer") {
                            VarCache::intCache[varName] = boost::lexical_cast<int64_t>(*result);
                            return true;
                        } else if(m_func.name == "string_to_decimal" || m_func.name == "decimal") {
                            VarCache::doubleCache[varName] = boost::lexical_cast<double>(*result);
                            return true;
                        }
                        return false;
                    } catch( boost::bad_lexical_cast const& ) {
                        setLastErrorMessage("string_to_: couldn't parse string literal.");
                        return false;
                    }
                }
            }
            return false;
        }

        
    };

}