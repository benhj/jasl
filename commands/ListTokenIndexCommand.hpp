//
//  ListTokenIndexCommand.hpp
//  jasl
//
//  Created by Ben Jones on 16/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"

namespace jasl
{
    class ListTokenIndexCommand : public Command
    {
    public:
        
        ListTokenIndexCommand(Function &func_,
                              SharedVarCache const &sharedCache = SharedVarCache(),
                              OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;

    private:

        OptionalString getTestToken();
        bool tryWithRawList(std::string const &varName);
        bool tryWithSymbolList(std::string const &varName);
    };

}