//
//  RandomCommand.cpp
//  jasl
//
//  Created by Ben Jones on 07/06/16
//  Copyright (c) 2016 Ben Jones. All rights reserved.
//

#include "RandomCommand.hpp"
#include "StringToPrimitiveCommand.hpp"
#include <cstdlib>
#include <random>

namespace jasl {

    RandomCommand::RandomCommand(Function &func_,
                                 SharedVarCache const &sharedCache,
                                 OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    , m_type()
    , m_varName()
    , m_r()
    {
        // random:int 5 -> a;
        // random:real 3.14 -> d;
        if(VarExtractor::tryAnyCast(m_type, m_func.paramA)) {
            if(!VarExtractor::tryAnyCast(m_varName, m_func.paramC)) {
                setLastErrorMessage("random: problem determining var name");
            }
        } else {
            setLastErrorMessage("random: problem determining random type");
        }
    }

    bool RandomCommand::execute()
    {
        if(m_varName.empty()) {
            setLastErrorMessage("random: var name empty. Can't continue.");
            return false;
        }

        static std::default_random_engine re(m_r());

        if (m_type == "int") {
            int64_t a;
            if (VarExtractor::trySingleIntExtraction(m_func.paramB, a, m_sharedCache)) {
                using Dist = std::uniform_int_distribution<int64_t>;
                static Dist uid {};
                auto output = uid(re, Dist::param_type{0, a});
                m_sharedCache->setVar(m_varName, output, Type::Int);
                return true;
            }
        } else if (m_type == "real") {
            double d;
            if (VarExtractor::trySingleRealExtraction(m_func.paramB, d, m_sharedCache)) {
                using Dist = std::uniform_real_distribution<double>;
                static Dist udd {};
                auto output = udd(re, Dist::param_type{0, d});
                m_sharedCache->setVar(m_varName, output, Type::Real);
                return true;
            }
        }

        setLastErrorMessage("random: bad type.");
        return false;
    }
}
