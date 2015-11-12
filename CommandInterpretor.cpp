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

#define BUILD_COMMAND_AND_EXECUTE(X)                   \
    [](Function &func,                              \
       SharedVarCache const &varCache,              \
       OptionalOutputStream const &outputStream) {  \
        X##Command c(func, varCache, outputStream); \
        return c.execute();                         \
    }

#define BUILD_COMMAND(X)                            \
    [](Function &func,                              \
       SharedVarCache const &varCache,              \
       OptionalOutputStream const &outputStream) {  \
        return std::make_shared<X##Command>(func, varCache, outputStream); \
    }

namespace jasl {

    /// Build the default set of commands
    CommandInterpretor::CommandMap CommandInterpretor::m_commandMap;
    CommandInterpretor::CommandBuilders CommandInterpretor::m_commandBuilders;

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
            m_commandMap.insert(std::make_pair("echo", BUILD_COMMAND_AND_EXECUTE(Echo)));
            m_commandMap.insert(std::make_pair("echo_nl", BUILD_COMMAND_AND_EXECUTE(EchoNL)));
            m_commandMap.insert(std::make_pair("if", BUILD_COMMAND_AND_EXECUTE(If)));
            m_commandMap.insert(std::make_pair("repeat", BUILD_COMMAND_AND_EXECUTE(Repeat)));
            m_commandMap.insert(std::make_pair("while", BUILD_COMMAND_AND_EXECUTE(While)));
            m_commandMap.insert(std::make_pair("for", BUILD_COMMAND_AND_EXECUTE(For)));
            m_commandMap.insert(std::make_pair("block", BUILD_COMMAND_AND_EXECUTE(Block)));
            m_commandMap.insert(std::make_pair("returnable", BUILD_COMMAND_AND_EXECUTE(Returnable)));
            m_commandMap.insert(std::make_pair("start", BUILD_COMMAND_AND_EXECUTE(Start)));
            m_commandMap.insert(std::make_pair("string", BUILD_COMMAND_AND_EXECUTE(String)));
            m_commandMap.insert(std::make_pair("call", BUILD_COMMAND_AND_EXECUTE(Call)));
            m_commandMap.insert(std::make_pair("append", BUILD_COMMAND_AND_EXECUTE(Append)));

            m_commandMap.insert(std::make_pair("string_reverse", BUILD_COMMAND_AND_EXECUTE(Reverse)));
            m_commandMap.insert(std::make_pair("string_length", BUILD_COMMAND_AND_EXECUTE(StringLength)));
            m_commandMap.insert(std::make_pair("list", BUILD_COMMAND_AND_EXECUTE(List)));
            m_commandMap.insert(std::make_pair("args", BUILD_COMMAND_AND_EXECUTE(Args)));
            m_commandMap.insert(std::make_pair("integer", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax)));
            m_commandMap.insert(std::make_pair("decimal", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax)));
            m_commandMap.insert(std::make_pair("boolean", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax)));
            m_commandMap.insert(std::make_pair("input", BUILD_COMMAND_AND_EXECUTE(Input)));
            m_commandMap.insert(std::make_pair("index_of", BUILD_COMMAND_AND_EXECUTE(ListTokenIndex)));
            m_commandMap.insert(std::make_pair("get_token", BUILD_COMMAND_AND_EXECUTE(ListGetToken)));
            m_commandMap.insert(std::make_pair("set_token", BUILD_COMMAND_AND_EXECUTE(ListSetToken)));

            m_commandMap.insert(std::make_pair("add_token", BUILD_COMMAND_AND_EXECUTE(ListAddToken)));
            m_commandMap.insert(std::make_pair("exec", BUILD_COMMAND_AND_EXECUTE(Exec)));
            m_commandMap.insert(std::make_pair("release", BUILD_COMMAND_AND_EXECUTE(Release)));
            m_commandMap.insert(std::make_pair("type", BUILD_COMMAND_AND_EXECUTE(Type)));
            m_commandMap.insert(std::make_pair("random_int", BUILD_COMMAND_AND_EXECUTE(RandomInt)));
            m_commandMap.insert(std::make_pair("exit", [](Function &,
                                                          SharedVarCache const &,              
                                                          OptionalOutputStream const &) {  
                                                              exit(0);
                                                              return false;
                                                          }));
        }
        if(m_commandBuilders.empty()) {
            m_commandBuilders.insert(std::make_pair("echo", BUILD_COMMAND(Echo)));
            m_commandBuilders.insert(std::make_pair("echo_nl", BUILD_COMMAND(EchoNL)));
            m_commandBuilders.insert(std::make_pair("if", BUILD_COMMAND(If)));
            m_commandBuilders.insert(std::make_pair("repeat", BUILD_COMMAND(Repeat)));
            m_commandBuilders.insert(std::make_pair("while", BUILD_COMMAND(While)));
            m_commandBuilders.insert(std::make_pair("for", BUILD_COMMAND(For)));
            m_commandBuilders.insert(std::make_pair("block", BUILD_COMMAND(Block)));
            m_commandBuilders.insert(std::make_pair("returnable", BUILD_COMMAND(Returnable)));
            m_commandBuilders.insert(std::make_pair("start", BUILD_COMMAND(Start)));
            m_commandBuilders.insert(std::make_pair("string", BUILD_COMMAND(String)));
            m_commandBuilders.insert(std::make_pair("call", BUILD_COMMAND(Call)));
            m_commandBuilders.insert(std::make_pair("append", BUILD_COMMAND(Append)));

            m_commandBuilders.insert(std::make_pair("string_reverse", BUILD_COMMAND(Reverse)));
            m_commandBuilders.insert(std::make_pair("string_length", BUILD_COMMAND(StringLength)));
            m_commandBuilders.insert(std::make_pair("list", BUILD_COMMAND(List)));
            m_commandBuilders.insert(std::make_pair("args", BUILD_COMMAND(Args)));
            m_commandBuilders.insert(std::make_pair("integer", BUILD_COMMAND(NewPrimitiveSyntax)));
            m_commandBuilders.insert(std::make_pair("decimal", BUILD_COMMAND(NewPrimitiveSyntax)));
            m_commandBuilders.insert(std::make_pair("boolean", BUILD_COMMAND(NewPrimitiveSyntax)));
            m_commandBuilders.insert(std::make_pair("input", BUILD_COMMAND(Input)));
            m_commandBuilders.insert(std::make_pair("index_of", BUILD_COMMAND(ListTokenIndex)));
            m_commandBuilders.insert(std::make_pair("get_token", BUILD_COMMAND(ListGetToken)));
            m_commandBuilders.insert(std::make_pair("set_token", BUILD_COMMAND(ListSetToken)));

            m_commandBuilders.insert(std::make_pair("add_token", BUILD_COMMAND(ListAddToken)));
            m_commandBuilders.insert(std::make_pair("exec", BUILD_COMMAND(Exec)));
            m_commandBuilders.insert(std::make_pair("release", BUILD_COMMAND(Release)));
            m_commandBuilders.insert(std::make_pair("type", BUILD_COMMAND(Type)));
            m_commandBuilders.insert(std::make_pair("random_int", BUILD_COMMAND(RandomInt)));
            m_commandBuilders.insert(std::make_pair("exit", [](Function &,
                                                          SharedVarCache const &,              
                                                          OptionalOutputStream const &) {  
                                                              exit(0);
                                                              return nullptr;
                                                          }));
        }
        
    }

    std::string
    CommandInterpretor::interpretFunc(Function &func,
                                      SharedVarCache const &varCache,
                                      OptionalOutputStream const &outputStream) const
    {
        return doInterpretFunc(func, varCache, outputStream);
    }

    std::string
    CommandInterpretor::doInterpretFunc(Function &func,
                                        SharedVarCache const &varCache,
                                        OptionalOutputStream const &outputStream) const
    {

        std::string errorMessage;
        try {
            m_commandMap[func.name](func, varCache, outputStream);
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
            return doInterpretFunc(func, varCache, outputStream);
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
        
        return parseStringCollection(script, varCache);
        
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

    std::shared_ptr<Command> 
    CommandInterpretor::funcToCommand(Function &func,
                                      SharedVarCache const &varCache,
                                      OptionalOutputStream const &outputStream) const
    {
        try {
            return m_commandBuilders[func.name](func, varCache, outputStream);
        } catch (...) {
            return nullptr;
        }
    }
}
