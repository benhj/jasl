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
#include "commands/CallCommand.hpp"
#include "commands/CVarCommand.hpp"
#include "commands/EchoCommand.hpp"
#include "commands/EchoNLCommand.hpp"
#include "commands/IfCommand.hpp"
#include "commands/ListCommand.hpp"
#include "commands/MathCommand.hpp"
#include "commands/NewPrimitiveSyntaxCommand.hpp"
#include "commands/RepeatCommand.hpp"
#include "commands/StartCommand.hpp"
#include "commands/VarCommand.hpp"
#include "commands/WhileCommand.hpp"
#include "commands/ReverseCommand.hpp"
#include "commands/StringCommand.hpp"
#include "commands/StringLengthCommand.hpp"
#include "commands/StringToPrimitiveCommand.hpp"

#include <boost/spirit/include/qi.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <iterator>

#define PROCESS_X_COMMAND(X)            \
    X##Command c(func, outputStream);   \
    (void)c.execute();                  \
    errorMessage = c.getErrorMessage();


namespace jasl {

    namespace {
        bool searchString(Function &func, std::string const &name)
        {
            return func.name == name;
        }
    }

    std::string
    CommandInterpretor::interpretFunc(Function &func,
                                      OptionalOutputStream const &outputStream) const
    {
        return doInterpretFunc(func, outputStream);
    }

    std::string
    CommandInterpretor::doInterpretFunc(Function &func,
                                        OptionalOutputStream const &outputStream) const
    {
        std::string errorMessage;
        if (searchString(func, "var")) {

            PROCESS_X_COMMAND(Var);

        } else if(searchString(func, "m_")) {

            PROCESS_X_COMMAND(Math);

        } else if(searchString(func, "int") ||
                  searchString(func, "double") ||
                  searchString(func, "bool")) {

            PROCESS_X_COMMAND(CVar);

        } else if(searchString(func, "echo")) {

            PROCESS_X_COMMAND(Echo);

        } else if(searchString(func, "echo_nl")) {

            PROCESS_X_COMMAND(EchoNL);

        }  else if(searchString(func, "if")) {

            PROCESS_X_COMMAND(If);

        } else if(searchString(func, "repeat")) {

            PROCESS_X_COMMAND(Repeat);

        } else if(searchString(func, "while")) {

            PROCESS_X_COMMAND(While);

        }  else if(searchString(func, "block")) {
            
            PROCESS_X_COMMAND(Block);

        } else if(searchString(func, "start")) {
            
            PROCESS_X_COMMAND(Start);

        } else if(searchString(func, "call")) {

            PROCESS_X_COMMAND(Call);

        } else if(searchString(func, "string")) {

            PROCESS_X_COMMAND(String);

        } else if(searchString(func, "string_append")) {

            PROCESS_X_COMMAND(Append);

        } else if(searchString(func, "string_reverse")) {

            PROCESS_X_COMMAND(Reverse);

        } else if(searchString(func, "string_length")) {

            PROCESS_X_COMMAND(StringLength);

        } else if(searchString(func, "string_to_integer") ||
                  searchString(func, "string_to_decimal")) {

            PROCESS_X_COMMAND(StringToPrimitive);

        } else if(searchString(func, "list")) {

            PROCESS_X_COMMAND(List);

        } else if(searchString(func, "args")) {

            PROCESS_X_COMMAND(Args);

        }  else if(searchString(func, "integer") ||
                  searchString(func, "decimal") ||
                  searchString(func, "boolean")) {

            PROCESS_X_COMMAND(NewPrimitiveSyntax);
        }
        if(errorMessage.empty()) { return std::string("Couldn't interpret function"); }
        return errorMessage;
    }
    
    std::string
    CommandInterpretor::parseAndInterpretSingleCommand(std::string const &cs,
                                                       OptionalOutputStream const &outputStream) const
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
            return doInterpretFunc(func, outputStream);
        } 
        return std::string("Unsuccessful parse");
    }

    std::vector<Function>
    CommandInterpretor::parseCommandFile(std::string const &path) const
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
        
        /*
        // wrap istream into iterator
        boost::spirit::istream_iterator begin(in);
        boost::spirit::istream_iterator end;

        bool result;
        while (begin != end) {
            result = boost::spirit::qi::phrase_parse(begin,
                                                     end,
                                                     functionGrammar,
                                                     space,
                                                     functions);
            if (!result) {
                break;
            }
        }

        // store the script in global static. Used to do block 
        // (jasl name for subroutine) lookups
        VarCache::script = stringCollection;
        return functions;*/
    }

    std::vector<Function>
    CommandInterpretor::parseStringCollection(std::string const &stringCollection) const
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