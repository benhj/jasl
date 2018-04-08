//
//  TokenizeCommand.cpp
//  jasl
//
//  Copyright (c) 2018-present Ben Jones. All rights reserved.
//

#include "TokenizeCommand.hpp"
#include "caching/VarExtractor.hpp"
#include <sstream>

namespace {
    StringArray getStrings(std::string const & str)
    {
        std::istringstream iss(str);
        return StringArray{std::istream_iterator<std::string>{iss},
                           std::istream_iterator<std::string>{}};
    }
}

namespace jasl
{
    TokenizeCommand::TokenizeCommand(Function &func_,
                                     SharedCacheStack const &sharedCache,
                                     OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool TokenizeCommand::execute() 
    {
        std::string listName;
        if(!m_func.getValueB<std::string>(listName, m_sharedCache)) {
            setLastErrorMessage("string: couldn't parse");
            return false;
        }

        if(tryLiteralExtraction(listName)) { return true; }
        if(trySymbolExtraction(listName)) { return true; }
        
        setLastErrorMessage("string: couldn't parse");
        return false;
    }

    bool TokenizeCommand::tryLiteralExtraction(std::string const &key)
    {
        LiteralString literalString;
        if(m_func.getValueA<LiteralString>(literalString, m_sharedCache)) {
            auto const literal = literalString.literal;
            auto strs = getStrings(literal);
            std::cout<<"Setting cache wth key "<<key<<std::endl;
            m_sharedCache->setVar(key, strs, Type::StringArray);
            return true;
        }
        return false;
    }

    bool TokenizeCommand::trySymbolExtraction(std::string const &key)
    {
        // Now try extracting a symbol
        std::string symbol;
        if(m_func.getValueA<std::string>(symbol, m_sharedCache)) {
            std::string value;
            if(m_sharedCache->getVar_(symbol, value, Type::String)) {
                auto strs = getStrings(value);
                m_sharedCache->setVar(key, strs, Type::StringArray);
                return true;
            }
        }
        return false;
    }
}
