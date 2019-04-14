//
//  jasl.cpp
//  jasl
//
//  Created by Ben Jones on 10/03/2015.
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "SimplePrompt.hpp"
#include "core/any.hpp"
#include "core/CommandInterpretor.hpp"
#include "core/LiteralString.hpp"
#include "caching/CacheStack.hpp"
#include "caching/GlobalCache.hpp"
#include "commands/CommandRegisterer.hpp"
#include "commands/Command.hpp"
#include <boost/progress.hpp>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>

namespace {
    std::string getWelcome()
    {
        return std::string("      ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄           \n")
                   .append("     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌          \n")
                   .append("      ▀▀▀▀▀█░█▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀▀▀ ▐░▌          \n")
                   .append("           ▐░▌    ▐░▌       ▐░▌▐░▌          ▐░▌          \n")
                   .append("           ▐░▌    ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ ▐░▌          \n")
                   .append("           ▐░▌    ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌          \n")
                   .append("           ▐░▌    ▐░█▀▀▀▀▀▀▀█░▌ ▀▀▀▀▀▀▀▀▀█░▌▐░▌          \n")
                   .append("           ▐░▌    ▐░▌       ▐░▌          ▐░▌▐░▌          \n")
                   .append("      ▄▄▄▄▄█░▌    ▐░▌       ▐░▌ ▄▄▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄▄▄ \n")
                   .append("     ▐░░░░░░░▌    ▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌    v0.1\n")
                   .append("      ▀▀▀▀▀▀▀      ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀▀▀▀▀▀▀▀▀▀▀ \n")
                   .append("\n")
                   .append("     Just Another Scripting Language")
                   .append("     (c) 2018 - present; Ben H.D. Jones.");
    }
}

int main (int argc , char *argv[])
{

    /* initialize random seed: */
    srand (time(NULL));

    namespace ll = jasl;
    ll::CommandInterpretor ci;

    jasl::registerAll();

    auto sharedCache = std::make_shared<ll::CacheStack>();
    sharedCache->setVar("argsCount", (int64_t)(argc - 2), ll::Type::Int);

    // When jasl is started without any arguments, start
    // the interactive interpreter
    if(argc <= 1) {

        // Simple prompt, as well as being hacky as fuck, it actually
        // buggy as fuck. Create a better prompt here, directly, using
        // a basic while true loop and getline.

        // Command prompt
        simpleprompt::SimplePrompt sp("", [&](std::string const & com){
            if(com == "quit") {
                exit(0);
            }
            ci.parseAndInterpretSingleCommand(com, sharedCache, std::cout);
        }, getWelcome(), "J> ");
        sp.start();

        // Alternative prompt -- might be less buggy but doesn't support
        // history and tab-complete.
        // std::cout << "\n\n" << std::endl;
        // std::cout << getWelcome() << std::endl;
        // std::cout << "\n\n" << std::endl;
        // while(true) {
        //     std::cout<<"J> ";
        //     std::string com;
        //     std::getline (std::cin, com);
        //     if (com == "quit") {
        //         exit(0);
        //     }
        //     ci.parseAndInterpretSingleCommand(com, sharedCache, std::cout);
        // }
    } 
    // Else interpret the supplied jasl source code
    else {

        // Store how many arguments there are
        // in special reserved int, 'argsCount'
        //ll::GlobalCache::setVar("argsCount", (int64_t)(argc - 2), ll::Type::Int);

        // store any command line arguments that should also be interpreted
        if(argc >= 2) {
            for(int i = 2; i < argc; ++i) {
                ll::GlobalCache::args.push_back(std::string(argv[i]));
            }
        }

        // parse input file
        auto functions = ci.parseCommandFile(argv[1]);

        // Now find all instances of 'script' and load extra script 'headers'
        // Note: can't use iterators due to modification while iterating.
        auto upper = functions.size();
        for (size_t n = 0; n < upper; ++n) {
            if(functions[n].name == "script") {
                auto const path = simple_any::any_cast<ll::LiteralString>(functions[n].paramA);
                ci.parseCommandFileAddToExisting(path.literal, functions);
                upper = functions.size();
            }
        }

        // Find start function
        ::boost::progress_timer t;
        for(auto &f : functions) {
            if(f.name == "start") {
                (void)ci.interpretFunc(f, sharedCache, std::cout);
                break;
            }
        }
    }
}
