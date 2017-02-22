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
        OpenSSL_add_all_algorithms();        /* Load cryptos, et.al. */
        SSL_load_error_strings();            /* Bring in and register error messages */
        auto method = SSLv2_client_method(); /* Create new client-method instance */
        return SSL_CTX_new(method);          /* Create new context */
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
        auto ssl = SSL_new(ctx);            /* create new SSL connection state */
        SSL_set_fd(ssl, fd);                /* attach the socket descriptor */
        if (SSL_connect(ssl) == -1) {       /* perform the connection */
            setLastErrorMessage("net_sread: ssl failure");
            return false;
        }
                              
        char recvBuff[1024];
        memset(recvBuff, '0',sizeof(recvBuff));
        int n = 0;
        std::vector<uint8_t> bytes;
        while ( (n = SSL_read(ssl, recvBuff, sizeof(recvBuff) - 1)) > 0) {
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