//
//  TCPConnectCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "TCPConnectCommand.hpp"
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

namespace jasl
{

    TCPConnectCommand::TCPConnectCommand(Function &func_,
                                               SharedCacheStack const &sharedCache,
                                               OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool TCPConnectCommand::execute() 
    {
        // now try and extract the actual words
        std::string server;
        int64_t port;
        if(!VarExtractor::trySingleStringExtraction(m_func.paramA, server, m_sharedCache)) {
            setLastErrorMessage("tcp_connect: can't extract server");
            return false;
        }
        if(!VarExtractor::trySingleIntExtraction(m_func.paramB, port, m_sharedCache)) {
            setLastErrorMessage("tcp_connect: can't extract port");
            return false;
        }

        // connect to server and port
        int64_t sockfd = 0;
        struct sockaddr_in serv_addr; 

        if((sockfd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            setLastErrorMessage("tcp_connect: couldn't create socket");
            return false;
        } 

        ::bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port); 
        auto serverAddress = ::gethostbyname(server.c_str());
        ::bcopy((char *)serverAddress->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr, serverAddress->h_length);

        if(::connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
           setLastErrorMessage("tcp_connect: connection failed");
           return false;
        } 

        std::string connectionName;
        if(!m_func.getValueC<std::string>(connectionName, m_sharedCache)) {
            setLastErrorMessage("tcp_connect: couldn't parse name");
            return false;
        }

        m_sharedCache->setVar(connectionName, sockfd, Type::Int);        
        return true;
    }
}