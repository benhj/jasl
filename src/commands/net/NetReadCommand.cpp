//
//  NetReadCommand.cpp
//  jasl
//
//  Copyright (c) 2017-present Ben Jones. All rights reserved.
//

#include "NetReadCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"

#include <curl/curl.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstdint>
#include <vector>

bool jasl::NetReadCommand::m_registered = 
registerCommand<jasl::NetReadCommand>();

namespace {

    inline
    size_t write_data(const char * data, size_t size, 
        size_t count, std::vector<uint8_t> * bytes) {
        for(int c = 0; c < count; ++c) {
            bytes->push_back(data[c]);
        }
        //stream->write(data, count);
        return size * count;
    }

    inline
    int pull_one_url(std::string const & url,
                     std::vector<uint8_t> & bytes)
    {
        CURL *curl;
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bytes);
            curl_easy_perform(curl);
            /* always cleanup */
            curl_easy_cleanup(curl);
            return 0;
        }
        return 1;
    }
}

namespace jasl
{
    NetReadCommand::NetReadCommand(Function &func_,
                                   SharedCacheStack const &sharedCache,
                                   OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    std::vector<std::string> NetReadCommand::getCommandNames()
    {
        return {"net_read"};
    }

    bool NetReadCommand::execute() 
    {

        std::string url;
        if(VarExtractor::trySingleStringExtraction(m_func.paramA, url, m_sharedCache)) {

            std::string bytesArrayName;
            if(!m_func.getValueB<std::string>(bytesArrayName, m_sharedCache)) {
                setLastErrorMessage("net_read: couldn't parse name");
                return false;
            }

            std::vector<uint8_t> bytes;
            if(pull_one_url(url, bytes) == 0) {
                m_sharedCache->setVar(bytesArrayName, bytes, Type::ByteArray);
                return true;
            }      
        }
        setLastErrorMessage("net_read: error");
        return false;
    }
}