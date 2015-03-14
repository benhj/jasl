//
//  InputCommand.cpp
//  jasl
//
//  Created by Ben Jones on 14/03/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "../LiteralString.hpp"
#include "../VarExtractor.hpp"
#include "../VarCache.hpp"
#include <iostream>

namespace jasl
{
    class InputCommand : public Command
    {
    public:
        InputCommand(Function &func_,
                    OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {

        }

        bool execute() override
        {
            LiteralString query;
            if(!m_func.getValueA<LiteralString>(query)) {
                m_errorMessage = "input: couldn't parse";
                appendToOutput(m_errorMessage);
                return false;
            }

            appendToOutput(query.literal);

            std::string answer;
            if(!m_func.getValueB<std::string>(answer)) {
                m_errorMessage = "input: couldn't parse answer variable";
                appendToOutput(m_errorMessage);
                return false;
            }

            std::getline(std::cin, VarCache::stringCache[answer]);

            return false;
        }

    };

}