//
//  ForCommand.hpp
//  jasl
//
//  Created by Ben Jones on 03/10/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"
#include <string>
#include <vector>

namespace jasl {

    class ForCommand : public Command
    {
    public:
        ForCommand(Function &func_, 
                   SharedCacheStack const &sharedCache = SharedCacheStack(),
                   OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~ForCommand();
        bool execute() override;
        static std::vector<std::string> getCommandNames();
    private:
        bool parseCommands(std::vector<Function> &functions);
        bool processList(List const &va, std::string const &listSymbol);

        template<typename T>
        bool processArray(T const &array, std::string const & listSymbol);

        bool processString(std::string const &str, 
                           std::string const & stringSymbol);

        static bool m_registered;
    };

}
