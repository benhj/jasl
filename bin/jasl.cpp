//
//  jasl.cpp
//  jasl
//
//  Created by Ben Jones on 10/03/2015.
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#include "../any.hpp"
#include "../CommandInterpretor.hpp"
#include "../LiteralString.hpp"
#include "../caching/CacheStack.hpp"
#include "../caching/GlobalCache.hpp"
#include <boost/progress.hpp>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>

int main (int argc , char *argv[])
{

    /* initialize random seed: */
    srand (time(NULL));

    namespace ll = jasl;
    ll::CommandInterpretor ci;

    if(argc <= 1) {
        std::cout<<"Error: please provide a jasl script"<<std::endl;
        exit(0);
    }  

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
    auto sharedCache = std::make_shared<ll::CacheStack>();
    sharedCache->setVar("argsCount", (int64_t)(argc - 2), ll::Type::Int);
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
