//
//  CommandInterpretor.hpp
//  lightlang
//
//  Created by Ben Jones on 07/03/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Function.hpp"
#include <string>
#include <vector>

namespace lightlang {
    
    class CommandInterpretor
    {
    public:
        /**
         * @brief Attempts to parse and interpret a single command
         * 
         * @param cs the string containing the command to be parsed
         * @return any error information
         */
        std::string
        parseAndInterpretSingleCommand(std::string const &cs)const;
        
        std::vector<Function>
        parseCommandFile(std::string const &path) const;
        
        std::vector<Function>
        parseStringCollection(std::string const &stringCollection) const;
        
    private:
        std::string
        interpretFunc(Function &func) const;
    };
    
}
