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
#include "commands/Command.hpp"
#include <boost/optional.hpp>
#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace jasl {
    
    class CommandInterpretor
    {

    public:

        using OptionalOutputStream = ::boost::optional<std::ostream&>;

        using CommandFunction = std::function<bool(Function &, 
                                                   SharedVarCache const &, 
                                                   OptionalOutputStream const &)>;
        using CommandBuilder = std::function<std::shared_ptr<Command>(Function &, 
                                                   SharedVarCache const &, 
                                                   OptionalOutputStream const &)>;

        using CommandMap = std::map<std::string, CommandFunction>;
        using CommandBuilders = std::map<std::string, CommandBuilder>;

        static CommandMap m_commandMap;
        static CommandBuilders m_commandBuilders;

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

        void parseCommandFileAddToExisting(std::string const &path,
                                           SharedVarCache const &varCache,
                                           std::vector<Function> &functions) const;
        
        std::vector<Function>
        parseStringCollection(std::string const &stringCollection, 
                              SharedVarCache const &varCache = SharedVarCache()) const;

        std::string
        interpretFunc(Function &func,
                      SharedVarCache const &varCache = SharedVarCache(),
                      OptionalOutputStream const &outputStream = OptionalOutputStream()) const;

        std::shared_ptr<Command> 
        funcToCommand(Function &func,
                      SharedVarCache const &varCache = SharedVarCache(),
                      OptionalOutputStream const &outputStream = OptionalOutputStream()) const;
        
    private:
        std::string
        doInterpretFunc(Function &func,
                        SharedVarCache const &varCache,
                        OptionalOutputStream const &outputStream) const;
    };
    
}
