//
//  OneOfCommand.cpp
//  jasl
//
//  Copyright (c) 2019 Ben Jones. All rights reserved.
//

#include "OneOfCommand.hpp"
#include "caching/VarExtractor.hpp"
#include "core/RegisterCommand.hpp"
#include <algorithm>
#include <string>
#include <random>
#include <iterator>

bool jasl::OneOfCommand::m_registered = 
registerCommand<jasl::OneOfCommand>();

namespace {
    // From an answer found on stack overflow.
    // https://stackoverflow.com/questions/
    // 6942273/how-to-get-a-random-element-from-a-c-container
    template<typename Iter, typename RandomGenerator>
    Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    }

    template<typename Iter>
    Iter select_randomly(Iter start, Iter end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return select_randomly(start, end, gen);
    }
}

namespace jasl
{
    OneOfCommand::OneOfCommand(Function &func_,
                             SharedCacheStack const &sharedCache,
                             OptionalOutputStream const &output)
    : Command(func_, sharedCache, output)
    {
    }

    OneOfCommand::~OneOfCommand() = default;

    std::vector<std::string> OneOfCommand::getCommandNames()
    {
        return {"oneof"};
    }

    bool OneOfCommand::execute() 
    {
        // Try to get list
        List list;
        if(VarExtractor::trySingleListExtraction(m_func.paramA, list, m_sharedCache)) {
            std::string varName;
            if(m_func.getValueB<std::string>(varName, m_sharedCache)) {
                if(!list.empty()) {
                    auto const element = *select_randomly(std::begin(list), std::end(list));
                    // try string
                    std::string ex;
                    if(VarExtractor::tryAnyCast<std::string>(ex, element)) {
                        m_sharedCache->setVar(varName, ex, Type::String);
                        return true;
                    }
                    List listex;
                    if(VarExtractor::tryAnyCast<List>(listex, element)) {
                        m_sharedCache->setVar(varName, listex, Type::List);
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
