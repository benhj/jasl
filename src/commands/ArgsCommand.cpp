//
//  ArgsCommand.cpp
//  jasl
//
//  Created by Ben Jones on 25/10/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//


#include "ArgsCommand.hpp"
#include "core/RegisterCommand.hpp"
#include "core/CommandInterpretor.hpp"
#include "caching/VarExtractor.hpp"
#include "caching/GlobalCache.hpp"
#include <sstream>
#include <cstdint>
#include <vector>

/**
 * AUTO TYPE REGISTRAION
 */
bool jasl::ArgsCommand::m_registered = registerCommand<jasl::ArgsCommand>();

namespace jasl
{ 
    ArgsCommand::ArgsCommand(Function &func_,
                             SharedCacheStack const &sharedCache,
                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    ArgsCommand::~ArgsCommand() = default;

    std::vector<std::string> ArgsCommand::getCommandNames()
    {
        return {"args"};
    }

    bool ArgsCommand::execute() 
    {

        int64_t value;
        if(!VarExtractor::trySingleIntExtraction(m_func.paramA, value, m_sharedCache)) {
            setLastErrorMessage("args: problem extracting int");
            return false;
        }

        std::string argString;
        if(!m_func.getValueB<std::string>(argString, m_sharedCache)) {
            setLastErrorMessage("args: couldn't parse argument string variable name");
            return false;
        }

        if(value < GlobalCache::args.size()) {
            auto argument(GlobalCache::args[value]);
            m_sharedCache->setVar(argString, argument, Type::String);
            return true;
        }

        return false;
    }
}
