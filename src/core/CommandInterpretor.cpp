//
//  CommandInterpretor.cpp
//  jasl
//
//  Created by Ben Jones
//  Copyright (c) 2015-present Ben Jones. All rights reserved.
//

#include "CommandInterpretor.hpp"
#include "CommandParser.hpp"
#include "caching/GlobalCache.hpp"
#include <boost/spirit/include/qi.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <iterator>

#define BUILD_COMMAND_AND_EXECUTE(X)                \
    [](Function &func,                              \
       SharedCacheStack const &varCache,            \
       OptionalOutputStream const &outputStream) {  \
        X##Command c(func, varCache, outputStream); \
        return c.execute();                         \
    }

#define BUILD_COMMAND(X)                            \
    [](Function &func,                              \
       SharedCacheStack const &varCache,            \
       OptionalOutputStream const &outputStream) {  \
        return std::make_shared<X##Command>(func, varCache, outputStream); \
    }

namespace {
    /// Make the parser static as an optimization. We will
    /// only ever have one ot these so this is ok, I think.
    using Parser = jasl::CommandParser;
    static Parser functionGrammar;
    
    void parseStringCollectionHelper(std::string const &stringCollection,
                                     jasl::SharedCacheStack const &varCache,
                                     std::vector<jasl::Function> &functions)
    {

        using boost::spirit::ascii::space;
        auto iter = std::begin(stringCollection);
        auto end = std::end(stringCollection);
        bool result;
        while (iter != end) {
            result = boost::spirit::qi::phrase_parse(iter,
                                                     end,
                                                     functionGrammar,
                                                     space,
                                                     functions);
            if (!result) {
                break;
            }
        }
    }

    std::string loadScriptFromFile(std::string const &path) 
    {
        using boost::spirit::ascii::space;

        // open file, disable skipping of whitespace
        std::ifstream in(path.c_str());
        in.unsetf(std::ios::skipws);

        std::string script("");

        std::string line;
        while (std::getline(in, line)) {
            line.append("\n");
            script.append(line);
        }
        return script;
    }
}

namespace jasl {

    /// Build the default set of commands

    CommandInterpretor::CommandMap & CommandInterpretor::getCommandMap()
    {
        static CommandMap commandMap;
        return commandMap;
    }
    CommandInterpretor::CommandBuilders & CommandInterpretor::getCommandBuilders()
    {
        static CommandBuilders commandBuilders;
        return commandBuilders;
    }

    CommandInterpretor::CommandInterpretor()
    {
        getCommandMap().emplace("exit", [](Function &,
                                        SharedCacheStack const &,
                                        OptionalOutputStream const &) {
                                            exit(0);
                                            return false;
                                        });
        
        getCommandBuilders().emplace("exit", [](Function &,
                                             SharedCacheStack const &,
                                             OptionalOutputStream const &) {
                                                 exit(0);
                                                 return nullptr;
                                             });
    }

    void CommandInterpretor::registerCommand(std::string name, CommandBuilder builder)
    {
        getCommandBuilders().emplace(name, builder);
        getCommandMap().emplace(name, [=](Function &func,
                                       SharedCacheStack const &varCache,
                                       OptionalOutputStream const &outputStream) {
            auto com = builder(func, varCache, outputStream);
            return com->execute();
        });
    }

    std::string
    CommandInterpretor::interpretFunc(Function &func,
                                      SharedCacheStack const &varCache,
                                      OptionalOutputStream const &outputStream) const
    {
        if(func.name == "script") {
            return "";
        }
        return doInterpretFunc(func, varCache, outputStream);
    }

    std::string
    CommandInterpretor::doInterpretFunc(Function &func,
                                        SharedCacheStack const &varCache,
                                        OptionalOutputStream const &outputStream) const
    {
        if(func.name == "script") {
            return "";
        }
        std::string errorMessage;
        try {
            getCommandMap()[func.name](func, varCache, outputStream);
            errorMessage = GlobalCache::lastKnownError;
            if(!errorMessage.empty()) { return std::string("Couldn't interpret function"); }
        } catch (...) {}
        return errorMessage;
    }

    bool CommandInterpretor::successfulParse(std::string const & str) const
    {
        using boost::spirit::ascii::space;
        auto iter = std::begin(str);
        auto end = std::end(str);
        Function func;
        return boost::spirit::qi::phrase_parse(iter, end, functionGrammar, space, func);
    }

    std::string
    CommandInterpretor::parseAndInterpretSingleCommand(std::string const &cs,
                                                       SharedCacheStack const &varCache,
                                                       OptionalOutputStream const &outputStream) const
    {

        using boost::spirit::ascii::space;
        auto iter = std::begin(cs);
        auto end = std::end(cs);
        Function func;
        bool successfulParse = boost::spirit::qi::phrase_parse(iter, end, functionGrammar, space, func);
        if (successfulParse) {
            return doInterpretFunc(func, varCache, outputStream);
        }
        return std::string("Unsuccessful parse");
    }

    std::vector<Function>
    CommandInterpretor::parseCommandFile(std::string const &path,
                                         SharedCacheStack const &varCache) const
    {
        auto script = loadScriptFromFile(path);
        return parseStringCollection(script, varCache);
    }

    void
    CommandInterpretor::parseCommandFileAddToExisting(std::string const &path,
                                                      std::vector<Function> &functions,
                                                      SharedCacheStack const &varCache) const
    {
        auto script = loadScriptFromFile(path);

        // append to the script in global static. 
        jasl::GlobalCache::script += script;

        parseStringCollectionHelper(script, varCache, functions);
    }

    std::vector<Function>
    CommandInterpretor::parseStringCollection(std::string const &stringCollection,
                                              SharedCacheStack const &varCache) const
    {
        // store the script in global static. 
        jasl::GlobalCache::script = stringCollection;
        std::vector<Function> functions;
        parseStringCollectionHelper(stringCollection, varCache, functions);
        return functions;
    }

    std::shared_ptr<Command>
    CommandInterpretor::funcToCommand(Function &func,
                                      SharedCacheStack const &varCache,
                                      OptionalOutputStream const &outputStream) const
    {
        try {
            return getCommandBuilders()[func.name](func, varCache, outputStream);
        } catch (...) {
            return nullptr;
        }
    }
}
