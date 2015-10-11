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
#include <cstdlib>

namespace jasl {
    class RandomIntCommand : public Command
    {
    public:
        RandomIntCommand(Function &func_,
                         SharedVarCache const &sharedCache = SharedVarCache(),
                         OptionalOutputStream const &output = OptionalOutputStream())
        : Command(func_, sharedCache, output)
        {
        }

        bool execute() override
        {

            auto a = VarExtractor::trySingleIntExtraction(m_func.paramA, m_sharedCache);
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

            m_sharedCache->setInt(key, output);
            return true;
        }
    };
}

