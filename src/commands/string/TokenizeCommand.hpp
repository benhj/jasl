//
// TokenizeCommand.hpp
// jasl
//
// Copyright (c) 2018-Present Ben Jones. All rights reserved.
//
// Tokenizes a string into individual words, storing them
// in a list
//
// tokenize "hello world" -> list;

#pragma once

#include "../Command.hpp"

namespace jasl
{
    class TokenizeCommand : public Command
    {
      public:
        TokenizeCommand(Function &func_,
                        SharedCacheStack const &sharedCache = SharedCacheStack(),
                        OptionalOutputStream const &output = OptionalOutputStream());

        bool execute() override;
      private:
        bool tryLiteralExtraction(std::string const &key);
        bool trySymbolExtraction(std::string const &key);
        static bool m_registered;
    };

}
