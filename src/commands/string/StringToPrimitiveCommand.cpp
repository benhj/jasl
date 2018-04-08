//
//  StringToPrimitiveCommand.cpp
//  jasl
//
//  Created by Ben Jones on 15/11/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "StringToPrimitiveCommand.hpp"
#include "other/LiteralString.hpp"
#include "caching/VarExtractor.hpp"
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <sstream>
#include <cstdint>

namespace jasl
{
    StringToPrimitiveCommand::StringToPrimitiveCommand(Function &func_,
                                                       SharedCacheStack const &sharedCache,
                                                       OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool StringToPrimitiveCommand::execute()
    {
        std::string varName;
        if(!m_func.getValueB<std::string>(varName, m_sharedCache)) {
            setLastErrorMessage("string_to_: couldn't parse int name");
            return false;
        }

        if(tryLiteralExtraction(varName)) { return true; }
        if(trySymbolExtraction(varName)) { return true; }

        return false;
    }

    bool StringToPrimitiveCommand::tryLiteralExtraction(std::string const &varName)
    {
        LiteralString literalString;
        if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
            try {
                // also handled off directy from NewPrimitiveSyntaxCommand so need
                // to also check if function name is int or real in respective branch
                if(m_func.name == "string_to_int" || m_func.name == "int") {
                    m_sharedCache->setVar(varName, boost::lexical_cast<int64_t>(literalString.literal), Type::Int);
                    return true;
                } else if(m_func.name == "string_to_real" || m_func.name == "real") {
                    m_sharedCache->setVar(varName, boost::lexical_cast<double>(literalString.literal), Type::Real);
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

    bool StringToPrimitiveCommand::trySymbolExtraction(std::string const &varName)
    {
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
            auto result = m_sharedCache->getVar<std::string>(symbol, Type::String);
            if(result) {
                try {
                    // also handled off directy from NewPrimitiveSyntaxCommand so need
                    // to also check if function name is int or real in respective branch
                    if(m_func.name == "string_to_int" || m_func.name == "int") {
                        m_sharedCache->setVar(varName, boost::lexical_cast<int64_t>(*result), Type::Int);
                        return true;
                    } else if(m_func.name == "string_to_real" || m_func.name == "real") {
                        m_sharedCache->setVar(varName, boost::lexical_cast<double>(*result), Type::Real);
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
}
