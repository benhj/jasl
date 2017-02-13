//
//  NetReadCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "NetReadCommand.hpp"
#include "../../caching/VarExtractor.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

namespace jasl
{

    NetReadCommand::NetReadCommand(Function &func_,
                                   SharedCacheStack const &sharedCache,
                                   OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool NetReadCommand::execute() 
    {
        // now try and extract the actual words
        int64_t fd;
        if(!VarExtractor::trySingleIntExtraction(m_func.paramA, fd, m_sharedCache)) {
            setLastErrorMessage("net_read: can't extract fd");
            return false;
        }

        char recvBuff[1024];
        memset(recvBuff, '0',sizeof(recvBuff));
        int n = 0;
        std::vector<uint8_t> bytes;
        while ( (n = ::read(fd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
            recvBuff[n] = 0;
            for (int i = 0; i < n; ++i) {
                bytes.push_back(recvBuff[i]);
            }
        } 

        if (n < 0) {
            setLastErrorMessage("net_read: read error");
            return false;
        }

        std::string bytesArrayName;
        if(!m_func.getValueB<std::string>(bytesArrayName, m_sharedCache)) {
            setLastErrorMessage("net_read: couldn't parse name");
            return false;
        }

        m_sharedCache->setVar(bytesArrayName, bytes, Type::ByteArray);        
        return true;
    }
}