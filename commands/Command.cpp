//
//  Command.cpp
//  jasl
//
//  Created by Ben Jones 25/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "Command.hpp"
#include "../GlobalCache.hpp"

namespace jasl
{
    Command::Command(Function &func_,
            SharedVarCache const &sharedCache,
            OptionalOutputStream const &output)
    : m_func(func_) 
    , m_sharedCache(std::move(sharedCache))
    , m_outputStream(std::move(output))
    {
    }

    void Command::setLastErrorMessage(std::string const &error)
    {
        GlobalCache::lastKnownError = error;
    }
}