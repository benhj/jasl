//
//  BytesToStringCommand.cpp
//  jasl
//
//  Copyright (c) 2018 Ben Jones. All rights reserved.
//

#include "BytesToStringCommand.hpp"
#include "core/LiteralString.hpp"
#include "caching/VarExtractor.hpp"
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <sstream>
#include <cstdint>

namespace jasl
{
    BytesToStringCommand::BytesToStringCommand(Function &func_,
                                               SharedCacheStack const &sharedCache,
                                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool BytesToStringCommand::execute()
    {
        std::string varName;
        if(!m_func.getValueB<std::string>(varName, m_sharedCache)) {
            setLastErrorMessage("bytes_to_string: couldn't parse name");
            return false;
        }

        ByteArray value;
        if(!VarExtractor::trySingleArrayExtraction(m_func.paramA, value, m_sharedCache, Type::ByteArray)) {
            setLastErrorMessage("bytes_to_string: problem with bytes array");
            return false;
        }
        std::string str{std::begin(value), std::end(value)};
        m_sharedCache->setVar(varName, str, Type::String);
        return true;
    }
}
