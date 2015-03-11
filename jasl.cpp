//
//  jasl.cpp
//  jasl
//
//  Created by Ben Jones on 10/03/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "CommandInterpretor.hpp"
#include <iostream>
#include <sstream>

int main (int argc , char *argv[])
{
    namespace ll = jasl;
    ll::CommandInterpretor ci;

    if(argc <= 1) {
        std::cout<<"Error: please provide a jasl script"<<std::endl;
        exit(0);
    }

    // parse input file
    std::stringstream ss;
    auto functions = ci.parseCommandFile(argv[1]);
    for(auto &f : functions) {
        if(f.name == "start") {
            (void)ci.interpretFunc(f, ss);
            break;
        }
    }

    // file has been parsed. Print out program output
    std::cout<<ss.str().c_str();

}