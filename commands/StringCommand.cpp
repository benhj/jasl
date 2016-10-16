//
//  StringCommand.cpp
//  jasl
//
//  Created by Ben Jones on 15/11/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "StringCommand.hpp"
#include "ListToStringCommand.hpp"
#include "../VarExtractor.hpp"
#include <sstream>

namespace jasl
{
    StringCommand::StringCommand(Function &func_,
                                 SharedVarCache const &sharedCache,
                                 OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool StringCommand::execute() 
    {
        std::string stringName;
        if(!m_func.getValueB<std::string>(stringName, m_sharedCache)) {
            setLastErrorMessage("string: couldn't parse");
            return false;
        }

        if(tryConcatExtraction(stringName)) { return true; }
        if(tryLiteralExtraction(stringName)) { return true; }
        if(trySymbolExtraction(stringName)) { return true; }
        if(tryNumericExtraction(stringName)) { return true; }
        if(tryListExtraction()) { return true; }
        
        setLastErrorMessage("string: couldn't parse");
        return false;
    }

    bool StringCommand::tryLiteralExtraction(std::string const &key)
    {
        LiteralString literalString;
        if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
            m_sharedCache->setString(key, literalString.literal);
            return true;
        }
        return false;
    }

    bool StringCommand::trySymbolExtraction(std::string const &key)
    {
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
            {
                int64_t value;
                if( m_sharedCache->getInt_(symbol, value)) {
                    m_sharedCache->setString(key, std::to_string(value));
                    return true;
                }
            }
            {
                double value;
                if(m_sharedCache->getDouble_(symbol, value)) {
                    m_sharedCache->setString(key, std::to_string(value));
                    return true;
                }
            }
            {
                bool value;
                if(m_sharedCache->getBool_(symbol, value)) {
                    m_sharedCache->setString(key, std::to_string(value));
                    return true;
                }
            }
            {
                std::string value;
                if(m_sharedCache->getString_(symbol, value)) {
                    m_sharedCache->setString(key, value);
                    return true;
                }
            }
            {
                ByteArray value;
                if(m_sharedCache->getByteArray_(symbol, value)) {

                    std::string str;
                    for (auto const & b : value) {
                        str += b;
                    }
                    m_sharedCache->setString(key, str);
                    return true;
                }
            }
        }

        return false;
    }

    bool StringCommand::tryNumericExtraction(std::string const &key)
    {

        {
            int64_t result;
            if(VarExtractor::trySingleIntExtractionNoMath(m_func.paramA, result, m_sharedCache)) {
                m_sharedCache->setString(key, std::to_string(result));
                return true;
            }
        }

        {
            double result;
            if(VarExtractor::tryToGetADouble(m_func.paramA, result, m_sharedCache)) {
                std::ostringstream ss;
                ss << result;
                m_sharedCache->setString(key, ss.str());
                return true;
            }
        }

        {
            bool result;
            if(VarExtractor::trySingleBoolExtraction(m_func.paramA, result, m_sharedCache)) {
                m_sharedCache->setString(key, std::to_string(result));
                return true;
            }
        }

        return false;

    }

    /// Note, currently, this only works with concatenating strings
    /// from string variables, i.e.
    /// string (a|b|c) -> str;
    /// and a,b,c all have to be strings
    bool StringCommand::tryConcatExtraction(std::string const & key)
    {

        StringArray array;
        if(m_func.getValueA<StringArray>(array, m_sharedCache)) {
            std::string result("");
            for (auto & v : array) {
                // try extracting symbols representing
                // strings and then concatenate them
                {
                    std::cout<<"v: "<<v<<std::endl;
                    std::string value;
                    if(m_sharedCache->getString_(v, value)) {
                        result.append(value);
                    }
                }
            }
            m_sharedCache->setString(key, result);
            return true;
        }
        return false;
    }

    bool StringCommand::tryListExtraction()
    {
        return ListToStringCommand(m_func, m_sharedCache, m_outputStream).execute();
    }
}
