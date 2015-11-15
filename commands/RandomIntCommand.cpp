//
//  RandomIntCommand.cpp
//  jasl
//
//  Created by Ben Jones on 15/11/15
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "RandomIntCommand.hpp"
#include "StringToPrimitiveCommand.hpp"
#include <cstdlib>

namespace jasl {

    RandomIntCommand::RandomIntCommand(Function &func_,
                                       SharedVarCache const &sharedCache,
                                       OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    bool RandomIntCommand::execute()
    {
        int64_t a;
        if (!VarExtractor::trySingleIntExtraction(m_func.paramA, a, m_sharedCache)) {
            setLastErrorMessage("random_int: problem determining parameter");
            return false;
        }

        auto output = (rand() % (int)(a + 1));

        std::string key;
        if(!VarExtractor::tryAnyCast(key, m_func.paramB)) {
            setLastErrorMessage("random_int: problem determining var name");
            return false;
        }

        m_sharedCache->setInt(key, output);
        return true;
    }
}
