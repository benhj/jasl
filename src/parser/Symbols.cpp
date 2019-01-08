#include "Symbols.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule< jasl::Symbols::Iterator, std::string(), ascii::space_type > jasl::Symbols::mathSymbols;
qi::rule< jasl::Symbols::Iterator, std::string(), ascii::space_type > jasl::Symbols::comparisonSymbols;

namespace jasl {
    void Symbols::init() 
    {
        using qi::lexeme;
        using ascii::string;

        // symbols to be used in a math expression
        mathSymbols %= lexeme[ string("%")
                             | string("+")
                             | string("-")
                             | string("/")
                             | string("*")
                             | string("^")];

        // logical symbols
        comparisonSymbols %= lexeme[ string("==")
                                   | string(">=")
                                   | string(">")
                                   | string("<=")
                                   | string("<")
                                   | string("&&")
                                   | string("||")
                                   | string("!=")
                                   | string("=")
                                   | string("/=")];
    }
}