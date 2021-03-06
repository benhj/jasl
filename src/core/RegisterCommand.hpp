#pragma once

#include "core/CommandInterpretor.hpp"
#include <string>
#include <vector>

template <typename T>
bool registerCommand() {
    auto builder = [](jasl::Function &func_,
                    jasl::SharedCacheStack const &sharedCache,
                    jasl::Command::OptionalOutputStream const &output) {
        return std::make_shared<T>(func_, sharedCache, output);
    };
    auto const names = T::getCommandNames();
    for(auto const & n : names) {
        jasl::CommandInterpretor::registerCommand(n, builder);
    }
    return true;
}