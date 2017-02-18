//
//  NetCloseCommand.cpp
//  jasl
//
//  Copyright (c) 2017 Ben Jones. All rights reserved.
//

#include "NetCloseCommand.hpp"
#include "../../caching/VarExtractor.hpp"

namespace jasl
{

    NetCloseCommand::NetCloseCommand(Function &func_,
                                     SharedCacheStack const &sharedCache,
                                     OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool NetCloseCommand::execute() 
    {
        // now try and extract the actual words
        int64_t fd;
        if(!VarExtractor::trySingleIntExtraction(m_func.paramA, fd, m_sharedCache)) {
            setLastErrorMessage("net_close: can't extract fd");
            return false;
        }

        ::close(fd);
      
        return true;
    }
}