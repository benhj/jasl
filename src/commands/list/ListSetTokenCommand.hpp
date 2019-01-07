//
//  ListSetTokenCommand.hpp
//  jasl
//
//  Created by Ben Jones on 18/03/15
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class ListSetTokenCommand : public Command
    {
    public:
        ListSetTokenCommand(Function &func_,
                            SharedCacheStack const &sharedCache = SharedCacheStack(),
                            OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
        static std::vector<std::string> getCommandNames();
    private:
        bool getIndex(int64_t&);
        bool getNewStringToken(std::string&);
        bool getNewVAToken(List&);
        bool tryWithRawList(std::string const &varName);
        bool tryWithSymbolList(std::string const &varName);
        static bool m_registered;
    };

}