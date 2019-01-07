//
//  AnsiUPCommand.cpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#include "AnsiUPCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <iostream>

namespace jasl
{
    AnsiUPCommand::AnsiUPCommand(Function &func_,
                                 SharedCacheStack const &sharedCache,
                                 OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool AnsiUPCommand::execute()
    {
        int64_t count;
        if (VarExtractor::trySingleIntExtraction(m_func.paramA, count, m_sharedCache)) {
            for(auto i = 0; i < count; ++i) {
                *(m_outputStream)<<"\x1b[A";
            }
            return true;
        }
        return false;
    }
}
