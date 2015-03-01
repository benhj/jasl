#include "CommandParser.hpp"
#include "commands/MainFunctionCommand.hpp"
#include <boost/spirit/include/qi.hpp>

#include <iostream>
#include <string>
#include <iterator>

#define SEARCH_STRING(X) func.name.find(##X) != std::string::npos

namespace lightlang {

    namespace {
        bool processMainFunctionCommand(CommandInterpretor &ci,
                                        Function &func,
                                        std::string &returnString)
        {
            MainFunctionCommand mfc(func, ci.se);
            bool success = mfc();
            returnString = mfc.returnString;
            return success;
        }
    }


    bool
    CommandInterpretor::interpretFunc(CommandFunction::Function &func)
    {
        std::string returnString;
        bool success=false;

        if (searchString(func, "main")) {
            success = processMainFunctionCommand(*this, func, returnString);
        } 

        return success;
    }


    returnStruct
    CommandInterpretor::parseCommandString(std::string cs)
    {

        using boost::spirit::ascii::space;
        typedef std::string::const_iterator iterator_type;
        typedef CommandFunction::CommandParser<iterator_type> commandParser;
        commandParser functionGrammar;
        auto iter = cs.begin();
        auto end = cs.end();
        Function func;
        bool successfulParse = boost::spirit::qi::phrase_parse(iter, end, functionGrammar, space, func);
        if (successfulParse) {
            interpretFunc(func);
        } 
    }

    std::vector<Function>
    CommandInterpretor::parseCommandFile(std::string const &path)
    {
        using boost::spirit::ascii::space;
        typedef boost::spirit::istream_iterator iterator_type;
        typedef CommandFunction::CommandParser<iterator_type> commandParser;
        commandParser functionGrammar;
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
    CommandInterpretor::parseStringCollection(std::string const &stringCollection)
    {
        using boost::spirit::ascii::space;
        typedef std::string::const_iterator iterator_type;
        typedef CommandFunction::CommandParser<iterator_type> commandParser;
        commandParser functionGrammar;
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