#pragma once

#include <map>
#include <openssl/ssl.h>

namespace jasl
{
    struct SSLMap { static std::map<int, SSL*> sslMap; };
}