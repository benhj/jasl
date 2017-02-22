//
//  NetSWriteCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "NetSWriteCommand.hpp"
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

    NetSWriteCommand::NetSWriteCommand(Function &func_,
                                      SharedCacheStack const &sharedCache,
                                      OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool NetSWriteCommand::execute() 
    {
        int64_t fd;
        if(!VarExtractor::trySingleIntExtraction(m_func.paramB, fd, m_sharedCache)) {
            setLastErrorMessage("net_write: can't extract fd");
            return false;
        }

        ByteArray value;
        if(!VarExtractor::trySingleArrayExtraction(m_func.paramA, value, m_sharedCache, Type::ByteArray)) {
            std::string tryString;
            if(VarExtractor::trySingleStringExtraction(m_func.paramA, tryString, m_sharedCache)) {
                ::write(fd, &tryString.front(), tryString.size());
                return true;
            }
            setLastErrorMessage("net_write: problem with bytes array");
            return false;
        } 

        auto ctx = InitCTX();
        auto ssl = SSL_new(ctx);            /* create new SSL connection state */
        SSL_set_fd(ssl, fd);                /* attach the socket descriptor */
        if (SSL_connect(ssl) == -1) {       /* perform the connection */
            setLastErrorMessage("net_swrite: ssl failure");
            return false;
        }
                              
        SSL_write(ssl, &value.front(), value.size());  
        return true;
    }
}