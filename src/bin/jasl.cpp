//
//  jasl.cpp
//  jasl
//
//  Created by Ben Jones on 10/03/2015.
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "SimplePrompt.hpp"
#include "other/any.hpp"
#include "other/CommandInterpretor.hpp"
#include "other/LiteralString.hpp"
#include "caching/CacheStack.hpp"
#include "caching/GlobalCache.hpp"
#include <boost/progress.hpp>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>

namespace {
    std::string getWelcome()
    {
        return std::string("    _____     _       ______   _____\n")
                   .append("   |_   _|   / \\    .' ____ \\ |_   _|\n")
                     .append("     | |    / _ \\   | (___ \\_|  | |\n")
                     .append("     | |   / ___ \\   _.____`.   | |   _ \n")
                .append("| |__' | _/ /   \\ \\_| \\____) | _| |__/ |\n")
                .append("`.____.'|____| |____|\\______.'|________|   v0.1");
    }
}


int main (int argc , char *argv[])
{

    /* initialize random seed: */
    srand (time(NULL));

    namespace ll = jasl;
    ll::CommandInterpretor ci;

    auto sharedCache = std::make_shared<ll::CacheStack>();
    sharedCache->setVar("argsCount", (int64_t)(argc - 2), ll::Type::Int);

    if(argc <= 1) {

        // Command prompt
        simpleprompt::SimplePrompt sp("", [&](std::string const & com){
            if(com == "quit") {
                exit(0);
            }
            ci.parseAndInterpretSingleCommand(com, sharedCache, std::cout);
        }, getWelcome(), "J> ");
        sp.start();

    } else {



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
