#include "CommandInterpretor.hpp"
#include "CommandParser.hpp"
#include "commands/CVarCommand.hpp"
#include "commands/EchoCommand.hpp"
#include "commands/IfCommand.hpp"
#include "commands/MathCommand.hpp"
#include "commands/VarCommand.hpp"

#include <boost/spirit/include/qi.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <iterator>

namespace lightlang {

    namespace {
        
        bool searchString(Function &func, std::string const &name)
        {
            return func.name.find(name) != std::string::npos;
        }
        
        std::string processVarCommand(Function &func,
                                      CommandInterpretor::OptionalOutputStream const &outputStream)
        {
            VarCommand vc(func, outputStream);
            (void)vc.execute();
            return vc.getErrorMessage();
        }

        std::string processMathCommand(Function &func,
                                      CommandInterpretor::OptionalOutputStream const &outputStream)
        {
            MathCommand mc(func, outputStream);
            (void)mc.execute();
            return mc.getErrorMessage();
        }

        std::string processCVarCommand(Function &func,
                                      CommandInterpretor::OptionalOutputStream const &outputStream)
        {
            CVarCommand vc(func, outputStream);
            (void)vc.execute();
            return vc.getErrorMessage();
        }

        std::string processEchoCommand(Function &func,
                                       CommandInterpretor::OptionalOutputStream const &outputStream)
        {
            EchoCommand vc(func, outputStream);
            (void)vc.execute();
            return vc.getErrorMessage();
        }

        std::string processIfCommand(Function &func,
                                       CommandInterpretor::OptionalOutputStream const &outputStream)
        {
            IfCommand vc(func, outputStream);
            (void)vc.execute();
            return vc.getErrorMessage();
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
        if (searchString(func, "var")) {

            return processVarCommand(func, outputStream);

        } else if(searchString(func, "m_")) {

            return processMathCommand(func, outputStream);

        } else if(searchString(func, "int") ||
                  searchString(func, "double") ||
                  searchString(func, "bool")) {

            return processCVarCommand(func, outputStream);

        } else if(searchString(func, "echo")) {

            return processEchoCommand(func, outputStream);

        } else if(searchString(func, "if")) {

            return processIfCommand(func, outputStream);

        }
        return std::string("Couldn't interpret function");
    }
    
    std::string
    CommandInterpretor::parseAndInterpretSingleCommand(std::string const &cs,
                                                       OptionalOutputStream const &outputStream) const
    {

        using boost::spirit::ascii::space;
        typedef std::string::const_iterator iterator_type;
        typedef CommandParser<iterator_type> Parser;
        Parser functionGrammar;
        auto iter = cs.begin();
        auto end = cs.end();
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

        return functions;
    }

    std::vector<Function>
    CommandInterpretor::parseStringCollection(std::string const &stringCollection) const
    {
        using boost::spirit::ascii::space;
        typedef std::string::const_iterator iterator_type;
        typedef CommandParser<iterator_type> Parser;
        Parser functionGrammar;
        auto iter = stringCollection.begin();
        auto end = stringCollection.end();
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