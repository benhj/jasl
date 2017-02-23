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
        auto method = SSLv23_method();
        return SSL_CTX_new(method); 
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
            setLastErrorMessage("net_swrite: can't extract fd");
            return false;
        }
        auto ctx = InitCTX();
        auto ssl = SSL_new(ctx);
        if (SSL_set_wfd(ssl, fd) == 0) {
            setLastErrorMessage("net_swrite: ssl failure");
            return false;
        }
        if (SSL_connect(ssl) == -1) {
            setLastErrorMessage("net_swrite: ssl failure");
            return false;
        }

        ByteArray value;
        if(!VarExtractor::trySingleArrayExtraction(m_func.paramA, value, m_sharedCache, Type::ByteArray)) {
            std::string tryString;
            if(VarExtractor::trySingleStringExtraction(m_func.paramA, tryString, m_sharedCache)) {
                SSL_write(ssl, &tryString.front(), tryString.size());
                return true;
            }
            setLastErrorMessage("net_swrite: problem with bytes array");
            return false;
        } 
           
        SSL_write(ssl, &value.front(), value.size());  
        SSL_free(ssl);
        SSL_CTX_free(ctx);

        return true;
    }
}