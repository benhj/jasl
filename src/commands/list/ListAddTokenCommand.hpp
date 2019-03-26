//
//  ListAddTokenCommand.hpp
//  jasl
//
//  Created by Ben Jones on 04/10/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"
#include <string>

namespace jasl
{
    class ListAddTokenCommand : public Command
    {
    public:
        ListAddTokenCommand(Function &func_,
                            SharedCacheStack const &sharedCache = SharedCacheStack(),
                            OptionalOutputStream const &output = OptionalOutputStream());
        virtual ~ListAddTokenCommand();
        bool execute() override;
        static std::vector<std::string> getCommandNames();
    private:
        OptionalString getNewStringToken();
        OptionalList getNewVAToken();
        bool tryWithSymbolList(std::string const &varName);
        static bool m_registered;
    };

}
