#include "Expressions.hpp"
#include "BasicTypes.hpp"
#include "Strings.hpp"
#include "Symbols.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule< jasl::Expressions::Iterator,
          jasl::MathExpression(),
          ascii::space_type> jasl::Expressions::mathExpression;
qi::rule< jasl::Expressions::Iterator,
          jasl::MathExpression(),
          ascii::space_type> jasl::Expressions::bracketedMathExpression;
qi::rule< jasl::Expressions::Iterator,
          jasl::ComparisonExpression(),
          ascii::space_type> jasl::Expressions::comparisonExpression;
qi::rule< jasl::Expressions::Iterator,
          jasl::ComparisonExpression(),
          ascii::space_type> jasl::Expressions::bracketedComparisonExpression;

namespace jasl {
    void Expressions::init() 
    {
        // a math expression enclose by brackets to aid compound expressions
        // e.g. (5.0 + 5.0)
        bracketedMathExpression %= '('
                                >> mathExpression
                                >> ')';

        // a simple math expression e.g. 5 + 5
        mathExpression %= (BasicTypes::doubleRule |
                           BasicTypes::intRule    |
                           Strings::genericString |
                           bracketedMathExpression)
                           >> Symbols::mathSymbols
                           >> (BasicTypes::doubleRule |
                               BasicTypes::intRule    |
                               Strings::genericString |
                               bracketedMathExpression);

        // a logical expression inside a pair of brackets e.g. (2 > 1)
        bracketedComparisonExpression  %= '('
                                       >> comparisonExpression
                                       >> ')';

        // a simple logical expression e.g 2 > 1
        comparisonExpression %= ( BasicTypes::doubleRule
                                | BasicTypes::intRule
                                | Strings::genericString
                                | BasicTypes::boolRule
                                | Strings::doubleQuotedString
                                | Strings::quotedString
                                | Strings::bracketedWords
                                | bracketedMathExpression
                                | bracketedComparisonExpression)
                             >> Symbols::comparisonSymbols
                             >> ( BasicTypes::doubleRule
                                | BasicTypes::intRule
                                | Strings::genericString
                                | BasicTypes::boolRule
                                | Strings::doubleQuotedString
                                | Strings::quotedString
                                | Strings::bracketedWords
                                | bracketedMathExpression
                                | bracketedComparisonExpression);
    }
}