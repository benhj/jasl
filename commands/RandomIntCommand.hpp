//
//  RandomIntCommand.hpp
//  jasl
//
//  Created by Ben Jones on 05/10/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Command.hpp"
#include "StringToPrimitiveCommand.hpp"
#include "../VarCache.hpp"
#include <cstdlib>

namespace jasl {
    class RandomIntCommand : public Command
    {
    public:
        RandomIntCommand(Function &func_,
                         OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, output)
        {
        }

        bool execute() override
        {

            auto a = VarExtractor::trySingleIntExtraction(m_func.paramA);
            if (!a) {
                setLastErrorMessage("random_int: problem determining parameter");
                return false;
            } 

            auto output = (rand() % (int)(*a + 1));

            std::string key;
            if(!VarExtractor::tryAnyCast(key, m_func.paramB)) {
                setLastErrorMessage("random_int: problem determining var name");
                return false;
            }

            VarCache::setInt(key, output);
            return true;
        }
    };
}

