#include "CommandInterpretor.hpp"
#include "CommandParser.hpp"
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
        
        void processVarCommand(Function &func)
        {
            //VarCommand vc(func);
            //vc.execute();
        }
    
    }

    void
    CommandInterpretor::interpretFunc(Function &func) const
    {
        if (searchString(func, "var")) {
            processVarCommand(func);
        } 
    }
    

    void
    CommandInterpretor::parseCommandString(std::string const &cs) const
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
            interpretFunc(func);
        } 
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