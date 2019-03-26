//
//  JoinAllCommand.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#include "JoinAllCommand.hpp"
#include "core/ThreadManager.hpp"
#include "core/RegisterCommand.hpp"

bool jasl::JoinAllCommand::m_registered = 
registerCommand<jasl::JoinAllCommand>();

namespace jasl {

    JoinAllCommand::JoinAllCommand(Function &func_,
                               SharedCacheStack const &sharedCache,
                               OptionalOutputStream const &output)
        : Command(func_, sharedCache, output)
    {
    }

    JoinAllCommand::~JoinAllCommand() = default;

    std::vector<std::string> JoinAllCommand::getCommandNames()
    {
        return {"join_all"};
    }

    bool JoinAllCommand::execute() 
    {
        ThreadManager::joinAll();
        return true;
    }
}
