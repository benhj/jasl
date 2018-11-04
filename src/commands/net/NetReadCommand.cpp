//
//  NetReadCommand.cpp
//  jasl
//
//  Copyright (c) 2017-present Ben Jones. All rights reserved.
//

#include "NetReadCommand.hpp"
#include "caching/VarExtractor.hpp"

#include <curl/curl.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

namespace {

    inline
    size_t write_data(const char * data, size_t size, 
        size_t count, std::vector<unit8_t> * bytes) {
        //stream->write(data, count);
        return size * count;
    }

    inline
    void pull_one_url(URL const & url,
                      std::vector<unit8_t> & bytes)
    {
        CURL *curl;
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.getWholeThing().c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bytes);
            curl_easy_perform(curl);
            /* always cleanup */
            curl_easy_cleanup(curl);
            stream->flush();
        }
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

    bool NetReadCommand::execute() 
    {

        std::string bytesArrayName;
        if(!m_func.getValueB<std::string>(bytesArrayName, m_sharedCache)) {
            setLastErrorMessage("net_read: couldn't parse name");
            return false;
        }

        m_sharedCache->setVar(bytesArrayName, bytes, Type::ByteArray);       
        return true;
    }
}