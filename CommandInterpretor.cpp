//
//  CommandInterpretor.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "CommandInterpretor.hpp"
#include "CommandParser.hpp"
#include "VarCache.hpp"
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

#define PROCESS_X_COMMAND(X)            \
    X##Command c(func, outputStream);   \
    (void)c.execute();                  \
    errorMessage = VarCache::lastKnownError;


namespace jasl {

    namespace {
        bool searchString(Function &func, std::string const &name)
        {
            return func.name == name;
        }
    }

    std::string
    CommandInterpretor::interpretFunc(Function &func,
                                      OptionalOutputStream const &outputStream,
                                      SharedVarCache const &varCache) const
    {
        return doInterpretFunc(func, outputStream, varCache);
    }

    std::string
    CommandInterpretor::doInterpretFunc(Function &func,
                                        OptionalOutputStream const &outputStream,
                                        SharedVarCache const &varCache) const
    {
        std::string errorMessage;
        if(searchString(func, "echo")) {

            PROCESS_X_COMMAND(Echo);

        } else if(searchString(func, "echo_nl")) {

            PROCESS_X_COMMAND(EchoNL);

        }  else if(searchString(func, "if")) {

            PROCESS_X_COMMAND(If);

        } else if(searchString(func, "repeat")) {

            PROCESS_X_COMMAND(Repeat);

        } else if(searchString(func, "while")) {

            PROCESS_X_COMMAND(While);

        } else if(searchString(func, "for")) {

            PROCESS_X_COMMAND(For);

        } else if(searchString(func, "block")) {
            
            PROCESS_X_COMMAND(Block);

        } else if(searchString(func, "returnable")) {
            
            PROCESS_X_COMMAND(Returnable);

        }  else if(searchString(func, "start")) {
            
            PROCESS_X_COMMAND(Start);

        } else if(searchString(func, "call")) {

            PROCESS_X_COMMAND(Call);

        } else if(searchString(func, "string")) {

            PROCESS_X_COMMAND(String);

        } else if(searchString(func, "append")) {

            PROCESS_X_COMMAND(Append);

        } else if(searchString(func, "string_reverse")) {

            PROCESS_X_COMMAND(Reverse);

        } else if(searchString(func, "string_length")) {

            PROCESS_X_COMMAND(StringLength);

        } else if(searchString(func, "list")) {

            PROCESS_X_COMMAND(List);

        } else if(searchString(func, "args")) {

            PROCESS_X_COMMAND(Args);

        }  else if(searchString(func, "integer") ||
                  searchString(func, "decimal") ||
                  searchString(func, "boolean")) {

            PROCESS_X_COMMAND(NewPrimitiveSyntax);

        } else if(searchString(func, "input")) {

            PROCESS_X_COMMAND(Input);

        } else if(searchString(func, "index_of")) {

            PROCESS_X_COMMAND(ListTokenIndex);

        } else if(searchString(func, "get_token")) {

            PROCESS_X_COMMAND(ListGetToken);

        } else if(searchString(func, "set_token")) {

            PROCESS_X_COMMAND(ListSetToken);

        } else if(searchString(func, "add_token")) {

            PROCESS_X_COMMAND(ListAddToken);

        }  else if(searchString(func, "exec")) {

            PROCESS_X_COMMAND(Exec);

        } else if(searchString(func, "release")) {

            PROCESS_X_COMMAND(Release);

        } else if(searchString(func, "type")) {

            PROCESS_X_COMMAND(Type);

        } else if(searchString(func, "random_int")) {

            PROCESS_X_COMMAND(RandomInt);

        }           
        if(errorMessage.empty()) { return std::string("Couldn't interpret function"); }
        return errorMessage;
    }
    
    std::string
    CommandInterpretor::parseAndInterpretSingleCommand(std::string const &cs,
                                                       OptionalOutputStream const &outputStream,
                                                       SharedVarCache const &varCache) const
    {

        using boost::spirit::ascii::space;
        typedef std::string::const_iterator iterator_type;
        typedef CommandParser<iterator_type> Parser;
        Parser functionGrammar;
        auto iter = std::begin(cs);
        auto end = std::end(cs);
        Function func;
        bool successfulParse = boost::spirit::qi::phrase_parse(iter, end, functionGrammar, space, func);
        if (successfulParse) {
            return doInterpretFunc(func, outputStream, varCache);
        } 
        return std::string("Unsuccessful parse");
    }

    std::vector<Function>
    CommandInterpretor::parseCommandFile(std::string const &path,
                                         SharedVarCache const &varCache) const
    {
        using boost::spirit::ascii::space;
        typedef boost::spirit::istream_iterator iterator_type;
        typedef CommandParser<iterator_type> Parser;
        Parser functionGrammar;
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
        return parseStringCollection(script);
        
    }

    std::vector<Function>
    CommandInterpretor::parseStringCollection(std::string const &stringCollection,
                                              SharedVarCache const &varCache) const
    {

        // store the script in global static. Used to do block 
        // (jasl name for subroutine) lookups
        VarCache::script = stringCollection;

        using boost::spirit::ascii::space;
        typedef std::string::const_iterator iterator_type;
        typedef CommandParser<iterator_type> Parser;
        Parser functionGrammar;
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