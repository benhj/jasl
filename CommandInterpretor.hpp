//
//  CommandInterpretor.hpp
//  jasl
//
//  Created by Ben Jones on 07/03/2015.
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#pragma once

#include "Function.hpp"
#include "SharedVarCache.hpp"
#include <boost/optional.hpp>
#include <ostream>
#include <string>
#include <vector>
#include <map>

namespace jasl {
    
    class CommandInterpretor
    {

    public:

        typedef ::boost::optional<std::ostream&> OptionalOutputStream;

        typedef std::function<bool(Function &, 
                                   SharedVarCache const &, 
                                   OptionalOutputStream const &)> CommandFunction;
        typedef std::map<std::string, CommandFunction> CommandMap;

        static CommandMap m_commandMap;

        CommandInterpretor();

        /**
         * @brief Attempts to parse and interpret a single command
         * 
         * @param cs the string containing the command to be parsed
         * @return any error information
         */
        std::string
        parseAndInterpretSingleCommand(std::string const &cs,
                                       SharedVarCache const &varCache = SharedVarCache(),
                                       OptionalOutputStream const &outputStream = OptionalOutputStream())const;
        
        std::vector<Function>
        parseCommandFile(std::string const &path, 
                         SharedVarCache const &varCache = SharedVarCache()) const;
        
        std::vector<Function>
        parseStringCollection(std::string const &stringCollection, 
                              SharedVarCache const &varCache = SharedVarCache()) const;

        std::string
        interpretFunc(Function &func,
                      SharedVarCache const &varCache = SharedVarCache(),
                      OptionalOutputStream const &outputStream = OptionalOutputStream()) const;
        
    private:
        std::string
        doInterpretFunc(Function &func,
                        SharedVarCache const &varCache,
                        OptionalOutputStream const &outputStream) const;
    };
    
}
