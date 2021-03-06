//
//  ListGetTokenCommand.hpp
//  jasl
//
//  Created by Ben Jones on 18/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"
#include <string>

namespace jasl
{
    class ListGetTokenCommand : public Command
    {
    public:
        ListGetTokenCommand(Function &func_,
                            SharedCacheStack const &sharedCache = SharedCacheStack(),
                            OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~ListGetTokenCommand();

        bool execute() override;
        static std::vector<std::string> getCommandNames();
    private:
        bool getIndex(int64_t&);
        bool tryWithRawList(std::string const &varName);
        bool tryWithSymbolList(std::string const &varName);
        static bool m_registered;
    };

}
