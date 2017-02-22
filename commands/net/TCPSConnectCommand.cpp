//
//  TCPSConnectCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "TCPSConnectCommand.hpp"
#include "../../caching/VarExtractor.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace {

    // Adapted slightly from 
    // https://www.cs.utah.edu/~swalton/listings/articles/ssl_client.c

    /*---------------------------------------------------------------------*/
    /*--- OpenConnection - create socket and connect to server.         ---*/
    /*---------------------------------------------------------------------*/
    int OpenConnection(const char *hostname, int port)
    {  
        struct hostent *host;
        struct sockaddr_in addr;

        if ( (host = ::gethostbyname(hostname)) == NULL ) {
            return -1;
        }
        auto sd = socket(PF_INET, SOCK_STREAM, 0);
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = *(long*)(host->h_addr);
        if ( ::connect(sd, (struct sockaddr *)&addr, sizeof(addr)) != 0 ) {
            close(sd);
            return -1;
        }
        return sd;
    }
}

namespace jasl
{

    TCPSConnectCommand::TCPSConnectCommand(Function &func_,
                                               SharedCacheStack const &sharedCache,
                                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool TCPSConnectCommand::execute() 
    {
        // now try and extract the actual words
        std::string server;
        int64_t port;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramA, server, m_sharedCache)) {
            setLastErrorMessage("tcp_sconnect: can't extract server");
            return false;
        }
        if(!VarExtractor::trySingleIntExtraction(m_func.paramB, port, m_sharedCache)) {
            setLastErrorMessage("tcp_sconnect: can't extract port");
            return false;
        }

        // connect to server and port
        auto sockfd = OpenConnection(server.c_str(), port);
        auto ssl = SSL_new(ctx);                /* create new SSL connection state */
        SSL_set_fd(ssl, sockfd);                /* attach the socket descriptor */
        if (SSL_connect(ssl) == -1) {       /* perform the connection */
            setLastErrorMessage("tcp_sconnect: ssl failure");
            return false;
        }
        
        std::string connectionName;
        if(!m_func.getValueC<std::string>(connectionName, m_sharedCache)) {
            setLastErrorMessage("tcp_sconnect: couldn't parse name");
            return false;
        }

        m_sharedCache->setVar(connectionName, (int64_t)sockfd, Type::Int);        
        return true;
    }
}