//
//  NetWriteCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "NetWriteCommand.hpp"
#include "../../caching/VarExtractor.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

namespace jasl
{

    NetWriteCommand::NetWriteCommand(Function &func_,
                                     SharedCacheStack const &sharedCache,
                                     OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool NetWriteCommand::execute() 
    {
        ByteArray value;
        if(!VarExtractor::trySingleArrayExtraction(m_func.paramA, value, m_sharedCache, Type::ByteArray)) {
            setLastErrorMessage("net_write: problem with bytes array");
            return false;
        }

        int64_t fd;
        if(!VarExtractor::trySingleIntExtraction(m_func.paramB, fd, m_sharedCache)) {
            setLastErrorMessage("net_write: can't extract fd");
            return false;
        }

        ::write(fd, &value.front(), value.size());
        return true;
    }
}