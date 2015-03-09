#include "CommandInterpretor.hpp"
#include "CommandParser.hpp"
#include "commands/CVarCommand.hpp"
#include "commands/VarCommand.hpp"
#include "commands/MathCommand.hpp"

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
        
        std::string processVarCommand(Function &func)
        {
            VarCommand vc(func);
            (void)vc.execute();
            return vc.errorMessage;
        }

        std::string processMathCommand(Function &func)
        {
            MathCommand mc(func);
            (void)mc.execute();
            return mc.errorMessage;
        }

        std::string processCVarCommand(Function &func)
        {
            CVarCommand vc(func);
            (void)vc.execute();
            return vc.errorMessage;
        }
    }

    std::string
    CommandInterpretor::interpretFunc(Function &func) const
    {
        if (searchString(func, "var")) {

            return processVarCommand(func);

        } else if(searchString(func, "m_")) {

            return processMathCommand(func);

        } else if(searchString(func, "int") ||
                  searchString(func, "double") ||
                  searchString(func, "bool")) {

            return processCVarCommand(func);
            
        }
        return std::string("Couldn't interpret function");
    }
    
    std::string
    CommandInterpretor::parseAndInterpretSingleCommand(std::string const &cs) const
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
            return interpretFunc(func);
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