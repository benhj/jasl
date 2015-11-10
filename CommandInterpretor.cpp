//
//  CommandInterpretor.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "CommandInterpretor.hpp"
#include "CommandParser.hpp"
#include "GlobalCache.hpp"
#include "commands/AppendCommand.hpp"
#include "commands/ArgsCommand.hpp"
#include "commands/BlockCommand.hpp"
#include "commands/ReturnableCommand.hpp"
#include "commands/CallCommand.hpp"
#include "commands/EchoCommand.hpp"
#include "commands/EchoNLCommand.hpp"
#include "commands/ExecCommand.hpp"
#include "commands/IfCommand.hpp"
#include "commands/InputCommand.hpp"
#include "commands/ListCommand.hpp"
#include "commands/ListGetTokenCommand.hpp"
#include "commands/ListSetTokenCommand.hpp"
#include "commands/ListAddTokenCommand.hpp"
#include "commands/ListToStringCommand.hpp"
#include "commands/ListTokenIndexCommand.hpp"
#include "commands/NewPrimitiveSyntaxCommand.hpp"
#include "commands/RepeatCommand.hpp"
#include "commands/StartCommand.hpp"
#include "commands/WhileCommand.hpp"
#include "commands/ForCommand.hpp"
#include "commands/ReverseCommand.hpp"
#include "commands/StringCommand.hpp"
#include "commands/StringLengthCommand.hpp"
#include "commands/StringToPrimitiveCommand.hpp"
#include "commands/ReleaseCommand.hpp"
#include "commands/TypeCommand.hpp"
#include "commands/RandomIntCommand.hpp"

#include <boost/spirit/include/qi.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <iterator>

#define BUILD_COMMAND_FUNCTION(X)                   \
    [](Function &func,                              \
       SharedVarCache const &varCache,              \
       OptionalOutputStream const &outputStream) {  \
        X##Command c(func, std::move(varCache), std::move(outputStream)); \
        return c.execute();                         \
    }

namespace jasl {

    /// Build the default set of commands
    CommandInterpretor::CommandMap CommandInterpretor::m_commandMap;

    /// Make the parser static as an optimization. We will
    /// only ever have one ot these so this is ok, I think.
    typedef std::string::const_iterator iterator_type;
    typedef CommandParser<iterator_type> Parser;
    static Parser functionGrammar;

    namespace {
        bool searchString(Function &func, std::string const &name)
        {
            return func.name == name;
        }
    }

    CommandInterpretor::CommandInterpretor()
    {
        // Populate the command map with  basic command.
        // TODO: ability to dynamically register new commands at runtime
        if(m_commandMap.empty()) {
            m_commandMap.insert(std::make_pair("echo", BUILD_COMMAND_FUNCTION(Echo)));
            m_commandMap.insert(std::make_pair("echo_nl", BUILD_COMMAND_FUNCTION(EchoNL)));
            m_commandMap.insert(std::make_pair("if", BUILD_COMMAND_FUNCTION(If)));
            m_commandMap.insert(std::make_pair("repeat", BUILD_COMMAND_FUNCTION(Repeat)));
            m_commandMap.insert(std::make_pair("while", BUILD_COMMAND_FUNCTION(While)));
            m_commandMap.insert(std::make_pair("for", BUILD_COMMAND_FUNCTION(For)));
            m_commandMap.insert(std::make_pair("block", BUILD_COMMAND_FUNCTION(Block)));
            m_commandMap.insert(std::make_pair("returnable", BUILD_COMMAND_FUNCTION(Returnable)));
            m_commandMap.insert(std::make_pair("start", BUILD_COMMAND_FUNCTION(Start)));
            m_commandMap.insert(std::make_pair("string", BUILD_COMMAND_FUNCTION(String)));
            m_commandMap.insert(std::make_pair("call", BUILD_COMMAND_FUNCTION(Call)));
            m_commandMap.insert(std::make_pair("append", BUILD_COMMAND_FUNCTION(Append)));

            m_commandMap.insert(std::make_pair("string_reverse", BUILD_COMMAND_FUNCTION(Reverse)));
            m_commandMap.insert(std::make_pair("string_length", BUILD_COMMAND_FUNCTION(StringLength)));
            m_commandMap.insert(std::make_pair("list", BUILD_COMMAND_FUNCTION(List)));
            m_commandMap.insert(std::make_pair("args", BUILD_COMMAND_FUNCTION(Args)));
            m_commandMap.insert(std::make_pair("integer", BUILD_COMMAND_FUNCTION(NewPrimitiveSyntax)));
            m_commandMap.insert(std::make_pair("decimal", BUILD_COMMAND_FUNCTION(NewPrimitiveSyntax)));
            m_commandMap.insert(std::make_pair("boolean", BUILD_COMMAND_FUNCTION(NewPrimitiveSyntax)));
            m_commandMap.insert(std::make_pair("input", BUILD_COMMAND_FUNCTION(Input)));
            m_commandMap.insert(std::make_pair("index_of", BUILD_COMMAND_FUNCTION(ListTokenIndex)));
            m_commandMap.insert(std::make_pair("get_token", BUILD_COMMAND_FUNCTION(ListGetToken)));
            m_commandMap.insert(std::make_pair("set_token", BUILD_COMMAND_FUNCTION(ListSetToken)));

            m_commandMap.insert(std::make_pair("add_token", BUILD_COMMAND_FUNCTION(ListAddToken)));
            m_commandMap.insert(std::make_pair("exec", BUILD_COMMAND_FUNCTION(Exec)));
            m_commandMap.insert(std::make_pair("release", BUILD_COMMAND_FUNCTION(Release)));
            m_commandMap.insert(std::make_pair("type", BUILD_COMMAND_FUNCTION(Type)));
            m_commandMap.insert(std::make_pair("random_int", BUILD_COMMAND_FUNCTION(RandomInt)));
            m_commandMap.insert(std::make_pair("exit", [](Function &,
                                                          SharedVarCache const &,              
                                                          OptionalOutputStream const &) {  
                                                              exit(0);
                                                              return false;
                                                          }));
        }
        
    }

    std::string
    CommandInterpretor::interpretFunc(Function &func,
                                      SharedVarCache const &varCache,
                                      OptionalOutputStream const &outputStream) const
    {
        return doInterpretFunc(func, std::move(varCache), std::move(outputStream));
    }

    std::string
    CommandInterpretor::doInterpretFunc(Function &func,
                                        SharedVarCache const &varCache,
                                        OptionalOutputStream const &outputStream) const
    {

        std::string errorMessage;
        try {
            m_commandMap[func.name](func, std::move(varCache), std::move(outputStream));
            errorMessage = GlobalCache::lastKnownError;
            if(errorMessage.empty()) { return std::string("Couldn't interpret function"); }
        } catch (...) {}
        return errorMessage;
    }
    
    std::string
    CommandInterpretor::parseAndInterpretSingleCommand(std::string const &cs,
                                                       SharedVarCache const &varCache,
                                                       OptionalOutputStream const &outputStream) const
    {

        using boost::spirit::ascii::space;
        auto iter = std::begin(cs);
        auto end = std::end(cs);
        Function func;
        bool successfulParse = boost::spirit::qi::phrase_parse(iter, end, functionGrammar, space, func);
        if (successfulParse) {
            return doInterpretFunc(func, std::move(varCache), std::move(outputStream));
        } 
        return std::string("Unsuccessful parse");
    }

    std::vector<Function>
    CommandInterpretor::parseCommandFile(std::string const &path,
                                         SharedVarCache const &varCache) const
    {
        using boost::spirit::ascii::space;
        std::vector<Function> functions;

        // open file, disable skipping of whitespace
        std::ifstream in(path.c_str());
        in.unsetf(std::ios::skipws);

        std::string script("");

        
        std::string line;
        while (std::getline(in, line)) {
            line.append("\n");
            script.append(line);
        }
        
        return parseStringCollection(std::move(script), std::move(varCache));
        
    }

    std::vector<Function>
    CommandInterpretor::parseStringCollection(std::string const &stringCollection,
                                              SharedVarCache const &varCache) const
    {


        // store the script in global static. Used to do block 
        // (jasl name for subroutine) lookups
        GlobalCache::script = stringCollection;

        using boost::spirit::ascii::space;
        auto iter = std::begin(stringCollection);
        auto end = std::end(stringCollection);
        std::vector<Function> functions;

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

        return functions;
    }
}
