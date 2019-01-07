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

#include "commands/ArgsCommand.hpp"
#include "commands/ExecCommand.hpp"

#include "commands/screenio/AnsiUPCommand.hpp"
#include "commands/screenio/EchoCommand.hpp"
#include "commands/screenio/EchoNLCommand.hpp"
#include "commands/screenio/InputCommand.hpp"

#include "commands/types/ArrayCommand.hpp"
#include "commands/types/ArrayPutCommand.hpp"
#include "commands/types/ArrayGetCommand.hpp"
#include "commands/types/NewPrimitiveSyntaxCommand.hpp"
#include "commands/types/PutCommand.hpp"
#include "commands/types/ReleaseCommand.hpp"
#include "commands/types/TypeCommand.hpp"

#include "commands/ReverseCommand.hpp"
#include "commands/string/StringCommand.hpp"
#include "commands/LengthCommand.hpp"
#include "commands/RandomCommand.hpp"

#include "commands/flow/BlockCommand.hpp"
#include "commands/flow/ReturnableCommand.hpp"
#include "commands/flow/CallCommand.hpp"
#include "commands/flow/IfCommand.hpp"
#include "commands/flow/RepeatCommand.hpp"
#include "commands/flow/StartCommand.hpp"
#include "commands/flow/WhileCommand.hpp"
#include "commands/flow/ForCommand.hpp"

#include "commands/list/ListCommand.hpp"
#include "commands/list/ListGetTokenCommand.hpp"
#include "commands/list/ListSetTokenCommand.hpp"
#include "commands/list/ListAddTokenCommand.hpp"
#include "commands/list/ListTokenIndexCommand.hpp"

#include "commands/string/AppendCommand.hpp"
#include "commands/string/ConcatCommand.hpp"
#include "commands/string/ListToStringCommand.hpp"
#include "commands/string/RegexEqCommand.hpp"
#include "commands/string/RegexParseCommand.hpp"
#include "commands/string/StringToPrimitiveCommand.hpp"
#include "commands/string/TokenizeCommand.hpp"
#include "commands/string/WildcardEqCommand.hpp"
#include "commands/string/WildcardParseCommand.hpp"

#include "commands/fileio/FolderListCommand.hpp"
#include "commands/fileio/FolderListRecursiveCommand.hpp"
#include "commands/fileio/FileReadBytesCommand.hpp"
#include "commands/fileio/FileReadLinesCommand.hpp"
#include "commands/fileio/FileWriteBytesCommand.hpp"
#include "commands/fileio/FileAppendBytesCommand.hpp"
#include "commands/fileio/FileAppendStringCommand.hpp"
#include "commands/fileio/FileTypeCommand.hpp"

#include "commands/net/NetReadCommand.hpp"

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
        //if(getCommandMap().empty()) 
        {
            getCommandMap().emplace("repeat", BUILD_COMMAND_AND_EXECUTE(Repeat));
            getCommandMap().emplace("while", BUILD_COMMAND_AND_EXECUTE(While));
            getCommandMap().emplace("for", BUILD_COMMAND_AND_EXECUTE(For));
            getCommandMap().emplace("block", BUILD_COMMAND_AND_EXECUTE(Block));
            getCommandMap().emplace("fn", BUILD_COMMAND_AND_EXECUTE(Returnable));
            // Hack due to two version of 'fn' in parser. The second one can fail
            // but keeps the 'fn' keyword, appended to the first one. At least
            // I think this is what's hapenning.
            getCommandMap().emplace("fnfn", BUILD_COMMAND_AND_EXECUTE(Returnable));
            getCommandMap().emplace("start", BUILD_COMMAND_AND_EXECUTE(Start));
            getCommandMap().emplace("string", BUILD_COMMAND_AND_EXECUTE(String));
            getCommandMap().emplace("concat", BUILD_COMMAND_AND_EXECUTE(Concat));
            getCommandMap().emplace("call", BUILD_COMMAND_AND_EXECUTE(Call));
            getCommandMap().emplace("append", BUILD_COMMAND_AND_EXECUTE(Append));
            getCommandMap().emplace("tokenize", BUILD_COMMAND_AND_EXECUTE(Tokenize));

            getCommandMap().emplace("string_reverse", BUILD_COMMAND_AND_EXECUTE(Reverse));
            getCommandMap().emplace("length", BUILD_COMMAND_AND_EXECUTE(Length));
            getCommandMap().emplace("list", BUILD_COMMAND_AND_EXECUTE(List));
            //getCommandMap().emplace("args", BUILD_COMMAND_AND_EXECUTE(Args));

            getCommandMap().emplace("ints", BUILD_COMMAND_AND_EXECUTE(Array));
            getCommandMap().emplace("reals", BUILD_COMMAND_AND_EXECUTE(Array));
            getCommandMap().emplace("bytes", BUILD_COMMAND_AND_EXECUTE(Array));
            getCommandMap().emplace("strings", BUILD_COMMAND_AND_EXECUTE(Array));

            getCommandMap().emplace("put", BUILD_COMMAND_AND_EXECUTE(Put));
            getCommandMap().emplace("get", BUILD_COMMAND_AND_EXECUTE(ArrayGet));
            getCommandMap().emplace("int", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax));
            getCommandMap().emplace("byte", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax));
            getCommandMap().emplace("real", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax));
            getCommandMap().emplace("bool", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax));
            getCommandMap().emplace("input", BUILD_COMMAND_AND_EXECUTE(Input));
            getCommandMap().emplace("index_of", BUILD_COMMAND_AND_EXECUTE(ListTokenIndex));
            getCommandMap().emplace("get_token", BUILD_COMMAND_AND_EXECUTE(ListGetToken));
            getCommandMap().emplace("set_token", BUILD_COMMAND_AND_EXECUTE(ListSetToken));

            getCommandMap().emplace("add_token", BUILD_COMMAND_AND_EXECUTE(ListAddToken));
            getCommandMap().emplace("exec", BUILD_COMMAND_AND_EXECUTE(Exec));
            getCommandMap().emplace("release", BUILD_COMMAND_AND_EXECUTE(Release));
            getCommandMap().emplace("type", BUILD_COMMAND_AND_EXECUTE(Type));
            getCommandMap().emplace("random", BUILD_COMMAND_AND_EXECUTE(Random));

            getCommandMap().emplace("folder_list", BUILD_COMMAND_AND_EXECUTE(FolderList));
            getCommandMap().emplace("folder_list_recursive", BUILD_COMMAND_AND_EXECUTE(FolderListRecursive));
            getCommandMap().emplace("file_read_bytes", BUILD_COMMAND_AND_EXECUTE(FileReadBytes));
            getCommandMap().emplace("file_read_lines", BUILD_COMMAND_AND_EXECUTE(FileReadLines));
            getCommandMap().emplace("file_write_bytes", BUILD_COMMAND_AND_EXECUTE(FileWriteBytes));
            getCommandMap().emplace("file_append_bytes", BUILD_COMMAND_AND_EXECUTE(FileAppendBytes));
            getCommandMap().emplace("file_append_line", BUILD_COMMAND_AND_EXECUTE(FileAppendString));
            getCommandMap().emplace("file_type", BUILD_COMMAND_AND_EXECUTE(FileType));

            getCommandMap().emplace("regex_eq", BUILD_COMMAND_AND_EXECUTE(RegexEq));
            getCommandMap().emplace("regex_parse", BUILD_COMMAND_AND_EXECUTE(RegexParse));
            getCommandMap().emplace("wildcard_eq", BUILD_COMMAND_AND_EXECUTE(WildcardEq));
            getCommandMap().emplace("wildcard_parse", BUILD_COMMAND_AND_EXECUTE(WildcardParse));

            getCommandMap().emplace("net_read", BUILD_COMMAND_AND_EXECUTE(NetRead));

            getCommandMap().emplace("ansi_up", BUILD_COMMAND_AND_EXECUTE(AnsiUP));

            getCommandMap().emplace("exit", [](Function &,
                                            SharedCacheStack const &,
                                            OptionalOutputStream const &) {
                                                exit(0);
                                                return false;
                                            });
        }
        //if(getCommandBuilders().empty()) 
        {
            getCommandBuilders().emplace("pr", BUILD_COMMAND(Echo));
            getCommandBuilders().emplace("prn", BUILD_COMMAND(EchoNL));
            getCommandBuilders().emplace("say", BUILD_COMMAND(Echo));
            getCommandBuilders().emplace("if", BUILD_COMMAND(If));
            getCommandBuilders().emplace("?", BUILD_COMMAND(If));
            getCommandBuilders().emplace("repeat", BUILD_COMMAND(Repeat));
            getCommandBuilders().emplace("while", BUILD_COMMAND(While));
            getCommandBuilders().emplace("for", BUILD_COMMAND(For));
            getCommandBuilders().emplace("block", BUILD_COMMAND(Block));
            getCommandBuilders().emplace("fn", BUILD_COMMAND(Returnable));
            // Hack due to two version of 'fn' in parser. The second one can fail
            // but keeps the 'fn' keyword, appended to the first one. At least
            // I think this is what's hapenning.
            getCommandBuilders().emplace("fnfn", BUILD_COMMAND(Returnable));
            getCommandBuilders().emplace("start", BUILD_COMMAND(Start));
            getCommandBuilders().emplace("string", BUILD_COMMAND(String));
            getCommandBuilders().emplace("concat", BUILD_COMMAND(Concat));
            getCommandBuilders().emplace("call", BUILD_COMMAND(Call));
            getCommandBuilders().emplace("append", BUILD_COMMAND(Append));
            getCommandBuilders().emplace("tokenize", BUILD_COMMAND(Tokenize));

            getCommandBuilders().emplace("string_reverse", BUILD_COMMAND(Reverse));
            getCommandBuilders().emplace("length", BUILD_COMMAND(Length));
            getCommandBuilders().emplace("list", BUILD_COMMAND(List));
            //getCommandBuilders().emplace("args", BUILD_COMMAND(Args));

            getCommandBuilders().emplace("ints", BUILD_COMMAND(Array));
            getCommandBuilders().emplace("reals", BUILD_COMMAND(Array));
            getCommandBuilders().emplace("bytes", BUILD_COMMAND(Array));
            getCommandBuilders().emplace("strings", BUILD_COMMAND(Array));

            getCommandBuilders().emplace("put", BUILD_COMMAND(Put));
            getCommandBuilders().emplace("get", BUILD_COMMAND(ArrayGet));
            getCommandBuilders().emplace("int", BUILD_COMMAND(NewPrimitiveSyntax));
            getCommandBuilders().emplace("byte", BUILD_COMMAND(NewPrimitiveSyntax));
            getCommandBuilders().emplace("real", BUILD_COMMAND(NewPrimitiveSyntax));
            getCommandBuilders().emplace("bool", BUILD_COMMAND(NewPrimitiveSyntax));
            getCommandBuilders().emplace("input", BUILD_COMMAND(Input));
            getCommandBuilders().emplace("index_of", BUILD_COMMAND(ListTokenIndex));
            getCommandBuilders().emplace("get_token", BUILD_COMMAND(ListGetToken));
            getCommandBuilders().emplace("set_token", BUILD_COMMAND(ListSetToken));

            getCommandBuilders().emplace("add_token", BUILD_COMMAND(ListAddToken));
            getCommandBuilders().emplace("exec", BUILD_COMMAND(Exec));
            getCommandBuilders().emplace("release", BUILD_COMMAND(Release));
            getCommandBuilders().emplace("type", BUILD_COMMAND(Type));
            getCommandBuilders().emplace("random", BUILD_COMMAND(Random));

            getCommandBuilders().emplace("folder_list", BUILD_COMMAND(FolderList));
            getCommandBuilders().emplace("folder_list_recursive", BUILD_COMMAND(FolderListRecursive));
            getCommandBuilders().emplace("file_read_bytes", BUILD_COMMAND(FileReadBytes));
            getCommandBuilders().emplace("file_read_lines", BUILD_COMMAND(FileReadLines));
            getCommandBuilders().emplace("file_write_bytes", BUILD_COMMAND(FileWriteBytes));
            getCommandBuilders().emplace("file_append_bytes", BUILD_COMMAND(FileAppendBytes));
            getCommandBuilders().emplace("file_append_line", BUILD_COMMAND(FileAppendString));

            getCommandBuilders().emplace("regex_eq", BUILD_COMMAND(RegexEq));
            getCommandBuilders().emplace("regex_parse", BUILD_COMMAND(RegexParse));
            getCommandBuilders().emplace("wildcard_eq", BUILD_COMMAND(WildcardEq));
            getCommandBuilders().emplace("wildcard_parse", BUILD_COMMAND(WildcardParse));

            getCommandBuilders().emplace("net_read", BUILD_COMMAND(NetRead));

            getCommandBuilders().emplace("ansi_up", BUILD_COMMAND(AnsiUP));

            getCommandBuilders().emplace("exit", [](Function &,
                                                 SharedCacheStack const &,
                                                 OptionalOutputStream const &) {
                                                     exit(0);
                                                     return nullptr;
                                                 });
        }

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
            if(errorMessage.empty()) { return std::string("Couldn't interpret function"); }
        } catch (...) {}
        return errorMessage;
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
