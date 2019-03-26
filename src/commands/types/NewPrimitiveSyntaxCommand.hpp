//
//  NewPrimitiveSyntaxCommand.hpp
//  jasl
//
//  Created by Ben Jones on 13/03/15
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#pragma once

#include "commands/Command.hpp"

namespace jasl {
    class NewPrimitiveSyntaxCommand : public Command
    {
      public:
        NewPrimitiveSyntaxCommand(Function &func_,
                                  SharedCacheStack const &sharedCache = SharedCacheStack(),
                                  OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
        static std::vector<std::string> getCommandNames();

        virtual ~NewPrimitiveSyntaxCommand();

      private:
        bool handleInt();
        bool handleReal();
        bool handleBool();
        bool handleByte();

        std::string m_type;
        std::string m_varName;
        static bool m_registered;
    };
}
