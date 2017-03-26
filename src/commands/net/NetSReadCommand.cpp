//
//  NetSReadCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "NetSReadCommand.hpp"
#include "StaticSSLMap.hpp"
#include "caching/VarExtractor.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <vector>

namespace {
    int getBytes(SSL* ssl, std::vector<uint8_t> &buffer) {
        char recvBuff[101];
        memset(recvBuff, '0',sizeof(recvBuff));
        int n = 0;
        if ( (n = SSL_read(ssl, recvBuff, sizeof(recvBuff) - 1)) > 0) {
            recvBuff[n] = 0;
            for (int i = 0; i < n; ++i) {
                buffer.push_back(recvBuff[i]);
            }
        }
        return n;
    }
}

namespace jasl
{

    NetSReadCommand::NetSReadCommand(Function &func_,
                                      SharedCacheStack const &sharedCache,
                                      OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool NetSReadCommand::execute() 
    {
        // now try and extract the actual words
        int64_t fd;
        if(!VarExtractor::trySingleIntExtraction(m_func.paramA, fd, m_sharedCache)) {
            setLastErrorMessage("net_sread: can't extract fd");
            return false;
        }


        auto ssl = SSLMap::sslMap[fd];      

        if(ssl) {
            char recvBuff[1024];
            memset(recvBuff, '0',sizeof(recvBuff));
            int n = 0;
            std::vector<uint8_t> bytes;

            // Simple heuristic. There's probably more bytes to received
            // if the recv buffer is saturated.
            while((n = getBytes(ssl, bytes)) == 100) {std::cout<<"n: "<<n<<std::endl;}
            std::cout<<"n: "<<n<<std::endl;

            if (n == 0) {
                setLastErrorMessage("net_sread: read error");
                return false;
            }

            std::string bytesArrayName;
            if(!m_func.getValueB<std::string>(bytesArrayName, m_sharedCache)) {
                setLastErrorMessage("net_sread: couldn't parse name");
                return false;
            }

            m_sharedCache->setVar(bytesArrayName, bytes, Type::ByteArray);        
            return true;
        }

        setLastErrorMessage("net_sread: invalid ssl context");
        return false;

    }
}