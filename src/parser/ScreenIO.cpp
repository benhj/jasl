#include "ScreenIO.hpp"
#include "BasicTypes.hpp"
#include "Strings.hpp"
#include "Expressions.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule<jasl::ScreenIO::Iterator, jasl::Function(), ascii::space_type> jasl::ScreenIO::echo;
qi::rule<jasl::ScreenIO::Iterator, jasl::Function(), ascii::space_type> jasl::ScreenIO::ansiUPRule;

namespace jasl {
    void ScreenIO::init() 
    {
        using qi::lexeme;
        using ascii::string;

        // for printing out a string to screen
        echo %= lexeme[string("prn") | string("pr") | string("say")]
             >> (Strings::doubleQuotedString |
                 Strings::genericString |
                 Strings::bracketedWords |
                 BasicTypes::doubleRule |
                 BasicTypes::intRule |
                 BasicTypes::boolRule |
                 Expressions::mathExpression |
                 Expressions::bracketedMathExpression |
                 Expressions::comparisonExpression |
                 Expressions::bracketedComparisonExpression)
             >> ';';

        ansiUPRule %= string("ansi_up")
                   >> (Strings::genericString
                      | BasicTypes::intRule
                      | Expressions::mathExpression
                      | Expressions::bracketedMathExpression) >> ';';

    }
}