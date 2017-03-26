//
//  TCPSConnectCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "TCPSConnectCommand.hpp"
#include "StaticSSLMap.hpp"
#include "caching/VarExtractor.hpp"

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

    // Based on code found at
    // https://www.cs.utah.edu/~swalton/listings/articles/ssl_client.c

    /*---------------------------------------------------------------------*/
    /*--- OpenConnection - create socket and connect to server.         ---*/
    /*---------------------------------------------------------------------*/
    int OpenConnection(const char *hostname, int port)
    {  
        int sockfd = 0;
        if((sockfd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            return -1;
        } 
        struct sockaddr_in serv_addr; 
        ::bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port); 
        auto serverAddress = ::gethostbyname(hostname);
        ::bcopy((char *)serverAddress->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr, serverAddress->h_length);

        if(::connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
           ::close(sockfd);
           return -1;
        } 
        return sockfd;
    }

    /*---------------------------------------------------------------------*/
    /*--- InitCTX - initialize the SSL engine.                          ---*/
    /*---------------------------------------------------------------------*/
    SSL_CTX* InitCTX(void)
    {
        SSL_load_error_strings();
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        auto method = SSLv23_client_method();
        return SSL_CTX_new(method); 
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

        if (sockfd == -1) {
            setLastErrorMessage("tcp_sconnect: bad descriptor");
            return false;
        }

        // create ssl connection and store in static map
        auto ctx = InitCTX();
        auto ssl = SSL_new(ctx);
        if (SSL_set_fd(ssl, sockfd) == 0) {
            setLastErrorMessage("tcp_sconnect: ssl failure");
            return false;
        }
        if (SSL_connect(ssl) == -1) {
            setLastErrorMessage("tcp_sconnect: ssl failure");
            return false;
        }
        SSLMap::sslMap.emplace(sockfd, ssl);

        std::string connectionName;
        if(!m_func.getValueC<std::string>(connectionName, m_sharedCache)) {
            setLastErrorMessage("tcp_sconnect: couldn't parse name");
            return false;
        }

        m_sharedCache->setVar(connectionName, (int64_t)sockfd, Type::Int);        
        return true;
    }
}