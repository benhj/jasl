//
//  CommandInterpretor.hpp
//  jasl
//
//  Created by Ben Jones on 07/03/2015.
//  Copyright (c) 2015-2016 Ben Jones. All rights reserved.
//

#pragma once

#include "Function.hpp"
#include "caching/CacheStack.hpp"
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
                                                   SharedCacheStack const &, 
                                                   OptionalOutputStream const &)>;
        using CommandBuilder = std::function<std::shared_ptr<Command>(Function &, 
                                                   SharedCacheStack const &, 
                                                   OptionalOutputStream const &)>;

        using CommandMap = std::map<std::string, CommandFunction>;
        using CommandBuilders = std::map<std::string, CommandBuilder>;
        using CommandRegistry = std::map<std::string, CommandBuilder>;

        // Static ordering issue
        static CommandMap & getCommandMap();
        static CommandBuilders & getCommandBuilders();

        CommandInterpretor();

        /// Register a command builder with a given name, with the registry
        static void registerCommand(std::string commandName,
                                    CommandBuilder builder);

        /**
         * @brief Attempts to parse and interpret a single command
         * 
         * @param cs the string containing the command to be parsed
         * @return any error information
         */
        std::string
        parseAndInterpretSingleCommand(std::string const &cs,
                                       SharedCacheStack const &varCache = SharedCacheStack(),
                                       OptionalOutputStream const &outputStream = OptionalOutputStream())const;
        
        std::vector<Function>
        parseCommandFile(std::string const &path, 
                         SharedCacheStack const &varCache = SharedCacheStack()) const;

        void parseCommandFileAddToExisting(std::string const &path,
                                           std::vector<Function> &functions,
                                           SharedCacheStack const &varCache  = SharedCacheStack()) const;
        
        std::vector<Function>
        parseStringCollection(std::string const &stringCollection, 
                              SharedCacheStack const &varCache = SharedCacheStack()) const;

        std::string
        interpretFunc(Function &func,
                      SharedCacheStack const &varCache = SharedCacheStack(),
                      OptionalOutputStream const &outputStream = OptionalOutputStream()) const;

        std::shared_ptr<Command> 
        funcToCommand(Function &func,
                      SharedCacheStack const &varCache = SharedCacheStack(),
                      OptionalOutputStream const &outputStream = OptionalOutputStream()) const;

        bool successfulParse(std::string const & str) const;
        
    private:
        std::string
        doInterpretFunc(Function &func,
                        SharedCacheStack const &varCache,
                        OptionalOutputStream const &outputStream) const;
    };
    
}
