//
//  CommandInterpretor.hpp
//  lightlang
//
//  Created by Ben Jones on 07/03/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Function.hpp"
#include <boost/optional.hpp>
#include <ostream>
#include <string>
#include <vector>

namespace lightlang {
    
    class CommandInterpretor
    {

    public:

        typedef ::boost::optional<std::ostream&> OptionalOutputStream;

        /**
         * @brief Attempts to parse and interpret a single command
         * 
         * @param cs the string containing the command to be parsed
         * @return any error information
         */
        std::string
        parseAndInterpretSingleCommand(std::string const &cs,
                                       OptionalOutputStream const &outputStream = OptionalOutputStream())const;
        
        std::vector<Function>
        parseCommandFile(std::string const &path) const;
        
        std::vector<Function>
        parseStringCollection(std::string const &stringCollection) const;

        std::string
        interpretFunc(Function &func,
                      OptionalOutputStream const &outputStream) const;
        
    private:
        std::string
        doInterpretFunc(Function &func,
                        OptionalOutputStream const &outputStream) const;
    };
    
}
