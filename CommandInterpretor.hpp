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
        void
        parseCommandString(std::string const &cs)const;
        
        std::vector<Function>
        parseCommandFile(std::string const &path) const;
        
        std::vector<Function>
        parseStringCollection(std::string const &stringCollection) const;
        
    private:
        void
        interpretFunc(Function &func) const;
    };
    
}
