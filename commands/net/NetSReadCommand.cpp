//
//  NetSReadCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "NetSReadCommand.hpp"
#include "../../caching/VarExtractor.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <vector>

namespace {
    /*---------------------------------------------------------------------*/
    /*--- InitCTX - initialize the SSL engine.                          ---*/
    /*---------------------------------------------------------------------*/
    SSL_CTX* InitCTX(void)
    {
        auto method = SSLv23_method();
        std::cout<<"4"<<std::endl;
        return SSL_CTX_new(method);    
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

        auto ctx = InitCTX();
        std::cout<<"A"<<std::endl;
        auto ssl = SSL_new(ctx);         
        std::cout<<"B"<<std::endl;
        if (SSL_set_rfd(ssl, fd) == 0) {
            setLastErrorMessage("net_sread: ssl failure");
            return false;
        }
          
        std::cout<<"C"<<std::endl;
        if (SSL_connect(ssl) == -1) {     
            setLastErrorMessage("net_sread: ssl failure");
            return false;
        }

        std::cout<<"D"<<std::endl;
        char recvBuff[1024];
        memset(recvBuff, '0',sizeof(recvBuff));
        int n = 0;
        std::vector<uint8_t> bytes;
        while ( (n = SSL_read(ssl, recvBuff, sizeof(recvBuff) - 1)) > 0) {
            std::cout<<"n: "<<n<<std::endl;
            recvBuff[n] = 0;
            for (int i = 0; i < n; ++i) {
                bytes.push_back(recvBuff[i]);
            }
        } 

        SSL_free(ssl);
        SSL_CTX_free(ctx);

        if (n < 0) {
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
}