//
//  CommandInterpretor.cpp
//  jasl
//
//  Created by Ben Jones
//  Copyright (c) 2015-2017 Ben Jones. All rights reserved.
//

#include "CommandInterpretor.hpp"
#include "CommandParser.hpp"
#include "caching/GlobalCache.hpp"
#include "commands/AppendCommand.hpp"
#include "commands/ArgsCommand.hpp"
#include "commands/ArrayCommand.hpp"
#include "commands/ArrayPutCommand.hpp"
#include "commands/ArrayGetCommand.hpp"
#include "commands/BlockCommand.hpp"
#include "commands/ReturnableCommand.hpp"
#include "commands/CallCommand.hpp"
#include "commands/ConcatCommand.hpp"
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
#include "commands/PutCommand.hpp"
#include "commands/MatchesCommand.hpp"
#include "commands/RepeatCommand.hpp"
#include "commands/StartCommand.hpp"
#include "commands/WhileCommand.hpp"
#include "commands/ForCommand.hpp"
#include "commands/ReverseCommand.hpp"
#include "commands/StringCommand.hpp"
#include "commands/LengthCommand.hpp"
#include "commands/StringToPrimitiveCommand.hpp"
#include "commands/ReleaseCommand.hpp"
#include "commands/TypeCommand.hpp"
#include "commands/RandomCommand.hpp"

#include "commands/fileio/FolderListCommand.hpp"
#include "commands/fileio/FolderListRecursiveCommand.hpp"
#include "commands/fileio/FileReadBytesCommand.hpp"
#include "commands/fileio/FileReadLinesCommand.hpp"
#include "commands/fileio/FileWriteBytesCommand.hpp"
#include "commands/fileio/FileAppendBytesCommand.hpp"
#include "commands/fileio/FileAppendStringCommand.hpp"
#include "commands/fileio/FileTypeCommand.hpp"

#include "commands/net/TCPConnectCommand.hpp"
#include "commands/net/NetReadCommand.hpp"
#include "commands/net/NetWriteCommand.hpp"
#include "commands/net/TCPSConnectCommand.hpp"
#include "commands/net/NetSReadCommand.hpp"
#include "commands/net/NetSWriteCommand.hpp"
#include "commands/net/NetCloseCommand.hpp"

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
    CommandInterpretor::CommandMap CommandInterpretor::m_commandMap;
    CommandInterpretor::CommandBuilders CommandInterpretor::m_commandBuilders;



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
            m_commandMap.emplace("pr", BUILD_COMMAND_AND_EXECUTE(Echo));
            m_commandMap.emplace("prn", BUILD_COMMAND_AND_EXECUTE(EchoNL));
            m_commandMap.emplace("say", BUILD_COMMAND_AND_EXECUTE(Echo));
            m_commandMap.emplace("if", BUILD_COMMAND_AND_EXECUTE(If));
            m_commandMap.emplace("?", BUILD_COMMAND_AND_EXECUTE(If));
            m_commandMap.emplace("repeat", BUILD_COMMAND_AND_EXECUTE(Repeat));
            m_commandMap.emplace("while", BUILD_COMMAND_AND_EXECUTE(While));
            m_commandMap.emplace("for", BUILD_COMMAND_AND_EXECUTE(For));
            m_commandMap.emplace("block", BUILD_COMMAND_AND_EXECUTE(Block));
            m_commandMap.emplace("fn", BUILD_COMMAND_AND_EXECUTE(Returnable));
            // Hack due to two version of 'fn' in parser. The second one can fail
            // but keeps the 'fn' keyword, appended to the first one. At least
            // I think this is what's hapenning.
            m_commandMap.emplace("fnfn", BUILD_COMMAND_AND_EXECUTE(Returnable));
            m_commandMap.emplace("start", BUILD_COMMAND_AND_EXECUTE(Start));
            m_commandMap.emplace("string", BUILD_COMMAND_AND_EXECUTE(String));
            m_commandMap.emplace("concat", BUILD_COMMAND_AND_EXECUTE(Concat));
            m_commandMap.emplace("call", BUILD_COMMAND_AND_EXECUTE(Call));
            m_commandMap.emplace("append", BUILD_COMMAND_AND_EXECUTE(Append));

            m_commandMap.emplace("string_reverse", BUILD_COMMAND_AND_EXECUTE(Reverse));
            m_commandMap.emplace("length", BUILD_COMMAND_AND_EXECUTE(Length));
            m_commandMap.emplace("list", BUILD_COMMAND_AND_EXECUTE(List));
            m_commandMap.emplace("args", BUILD_COMMAND_AND_EXECUTE(Args));

            m_commandMap.emplace("ints", BUILD_COMMAND_AND_EXECUTE(Array));
            m_commandMap.emplace("reals", BUILD_COMMAND_AND_EXECUTE(Array));
            m_commandMap.emplace("bytes", BUILD_COMMAND_AND_EXECUTE(Array));
            m_commandMap.emplace("strings", BUILD_COMMAND_AND_EXECUTE(Array));

            m_commandMap.emplace("put", BUILD_COMMAND_AND_EXECUTE(Put));
            m_commandMap.emplace("get", BUILD_COMMAND_AND_EXECUTE(ArrayGet));
            m_commandMap.emplace("int", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax));
            m_commandMap.emplace("byte", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax));
            m_commandMap.emplace("real", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax));
            m_commandMap.emplace("bool", BUILD_COMMAND_AND_EXECUTE(NewPrimitiveSyntax));
            m_commandMap.emplace("input", BUILD_COMMAND_AND_EXECUTE(Input));
            m_commandMap.emplace("index_of", BUILD_COMMAND_AND_EXECUTE(ListTokenIndex));
            m_commandMap.emplace("get_token", BUILD_COMMAND_AND_EXECUTE(ListGetToken));
            m_commandMap.emplace("set_token", BUILD_COMMAND_AND_EXECUTE(ListSetToken));

            m_commandMap.emplace("add_token", BUILD_COMMAND_AND_EXECUTE(ListAddToken));
            m_commandMap.emplace("exec", BUILD_COMMAND_AND_EXECUTE(Exec));
            m_commandMap.emplace("release", BUILD_COMMAND_AND_EXECUTE(Release));
            m_commandMap.emplace("type", BUILD_COMMAND_AND_EXECUTE(Type));
            m_commandMap.emplace("random", BUILD_COMMAND_AND_EXECUTE(Random));

            m_commandMap.emplace("folder_list", BUILD_COMMAND_AND_EXECUTE(FolderList));
            m_commandMap.emplace("folder_list_recursive", BUILD_COMMAND_AND_EXECUTE(FolderListRecursive));
            m_commandMap.emplace("file_read_bytes", BUILD_COMMAND_AND_EXECUTE(FileReadBytes));
            m_commandMap.emplace("file_read_lines", BUILD_COMMAND_AND_EXECUTE(FileReadLines));
            m_commandMap.emplace("file_write_bytes", BUILD_COMMAND_AND_EXECUTE(FileWriteBytes));
            m_commandMap.emplace("file_append_bytes", BUILD_COMMAND_AND_EXECUTE(FileAppendBytes));
            m_commandMap.emplace("file_append_line", BUILD_COMMAND_AND_EXECUTE(FileAppendString));
            m_commandMap.emplace("file_type", BUILD_COMMAND_AND_EXECUTE(FileTypeString));

            m_commandMap.emplace("matches", BUILD_COMMAND_AND_EXECUTE(Matches));

            m_commandMap.emplace("tcp_connect", BUILD_COMMAND_AND_EXECUTE(TCPConnect));
            m_commandMap.emplace("net_read", BUILD_COMMAND_AND_EXECUTE(NetRead));
            m_commandMap.emplace("net_write", BUILD_COMMAND_AND_EXECUTE(NetWrite));
            m_commandMap.emplace("tcp_sconnect", BUILD_COMMAND_AND_EXECUTE(TCPSConnect));
            m_commandMap.emplace("net_sread", BUILD_COMMAND_AND_EXECUTE(NetSRead));
            m_commandMap.emplace("net_swrite", BUILD_COMMAND_AND_EXECUTE(NetSWrite));
            m_commandMap.emplace("net_close", BUILD_COMMAND_AND_EXECUTE(NetClose));

            m_commandMap.emplace("exit", [](Function &,
                                            SharedCacheStack const &,
                                            OptionalOutputStream const &) {
                                                exit(0);
                                                return false;
                                            });
        }
        if(m_commandBuilders.empty()) {
            m_commandBuilders.emplace("pr", BUILD_COMMAND(Echo));
            m_commandBuilders.emplace("prn", BUILD_COMMAND(EchoNL));
            m_commandBuilders.emplace("say", BUILD_COMMAND(Echo));
            m_commandBuilders.emplace("if", BUILD_COMMAND(If));
            m_commandBuilders.emplace("?", BUILD_COMMAND(If));
            m_commandBuilders.emplace("repeat", BUILD_COMMAND(Repeat));
            m_commandBuilders.emplace("while", BUILD_COMMAND(While));
            m_commandBuilders.emplace("for", BUILD_COMMAND(For));
            m_commandBuilders.emplace("block", BUILD_COMMAND(Block));
            m_commandBuilders.emplace("fn", BUILD_COMMAND(Returnable));
            // Hack due to two version of 'fn' in parser. The second one can fail
            // but keeps the 'fn' keyword, appended to the first one. At least
            // I think this is what's hapenning.
            m_commandBuilders.emplace("fnfn", BUILD_COMMAND(Returnable));
            m_commandBuilders.emplace("start", BUILD_COMMAND(Start));
            m_commandBuilders.emplace("string", BUILD_COMMAND(String));
            m_commandBuilders.emplace("concat", BUILD_COMMAND(Concat));
            m_commandBuilders.emplace("call", BUILD_COMMAND(Call));
            m_commandBuilders.emplace("append", BUILD_COMMAND(Append));

            m_commandBuilders.emplace("string_reverse", BUILD_COMMAND(Reverse));
            m_commandBuilders.emplace("length", BUILD_COMMAND(Length));
            m_commandBuilders.emplace("list", BUILD_COMMAND(List));
            m_commandBuilders.emplace("args", BUILD_COMMAND(Args));

            m_commandBuilders.emplace("ints", BUILD_COMMAND(Array));
            m_commandBuilders.emplace("reals", BUILD_COMMAND(Array));
            m_commandBuilders.emplace("bytes", BUILD_COMMAND(Array));
            m_commandBuilders.emplace("strings", BUILD_COMMAND(Array));

            m_commandBuilders.emplace("put", BUILD_COMMAND(Put));
            m_commandBuilders.emplace("get", BUILD_COMMAND(ArrayGet));
            m_commandBuilders.emplace("int", BUILD_COMMAND(NewPrimitiveSyntax));
            m_commandBuilders.emplace("byte", BUILD_COMMAND(NewPrimitiveSyntax));
            m_commandBuilders.emplace("real", BUILD_COMMAND(NewPrimitiveSyntax));
            m_commandBuilders.emplace("bool", BUILD_COMMAND(NewPrimitiveSyntax));
            m_commandBuilders.emplace("input", BUILD_COMMAND(Input));
            m_commandBuilders.emplace("index_of", BUILD_COMMAND(ListTokenIndex));
            m_commandBuilders.emplace("get_token", BUILD_COMMAND(ListGetToken));
            m_commandBuilders.emplace("set_token", BUILD_COMMAND(ListSetToken));

            m_commandBuilders.emplace("add_token", BUILD_COMMAND(ListAddToken));
            m_commandBuilders.emplace("exec", BUILD_COMMAND(Exec));
            m_commandBuilders.emplace("release", BUILD_COMMAND(Release));
            m_commandBuilders.emplace("type", BUILD_COMMAND(Type));
            m_commandBuilders.emplace("random", BUILD_COMMAND(Random));

            m_commandBuilders.emplace("folder_list", BUILD_COMMAND(FolderList));
            m_commandBuilders.emplace("folder_list_recursive", BUILD_COMMAND(FolderListRecursive));
            m_commandBuilders.emplace("file_read_bytes", BUILD_COMMAND(FileReadBytes));
            m_commandBuilders.emplace("file_read_lines", BUILD_COMMAND(FileReadLines));
            m_commandBuilders.emplace("file_write_bytes", BUILD_COMMAND(FileWriteBytes));
            m_commandBuilders.emplace("file_append_bytes", BUILD_COMMAND(FileAppendBytes));
            m_commandBuilders.emplace("file_append_line", BUILD_COMMAND(FileAppendString));

            m_commandBuilders.emplace("matches", BUILD_COMMAND(Matches));

            m_commandBuilders.emplace("tcp_connect", BUILD_COMMAND(TCPConnect));
            m_commandBuilders.emplace("net_read", BUILD_COMMAND(NetRead));
            m_commandBuilders.emplace("net_write", BUILD_COMMAND(NetWrite));
            m_commandBuilders.emplace("tcp_sconnect", BUILD_COMMAND(TCPSConnect));
            m_commandBuilders.emplace("net_sread", BUILD_COMMAND(NetSRead));
            m_commandBuilders.emplace("net_swrite", BUILD_COMMAND(NetSWrite));
            m_commandBuilders.emplace("net_close", BUILD_COMMAND(NetClose));

            m_commandBuilders.emplace("exit", [](Function &,
                                                 SharedCacheStack const &,
                                                 OptionalOutputStream const &) {
                                                     exit(0);
                                                     return nullptr;
                                                 });
        }

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
            m_commandMap[func.name](func, varCache, outputStream);
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
            return m_commandBuilders[func.name](func, varCache, outputStream);
        } catch (...) {
            return nullptr;
        }
    }
}
