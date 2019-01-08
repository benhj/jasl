#include "Parameters.hpp"
#include "Strings.hpp"
#include "BasicTypes.hpp"
#include "Expressions.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule< jasl::Parameters::Iterator, Value(), ascii::space_type> jasl::Parameters::parameter;
qi::rule< jasl::Parameters::Iterator, Value(), ascii::space_type> jasl::Parameters::commaParameter;
qi::rule< jasl::Parameters::Iterator, Value(), ascii::space_type> jasl::Parameters::parameterList;
qi::rule< jasl::Parameters::Iterator, ValueArray(), ascii::space_type> jasl::Parameters::parameters;

namespace jasl {
    void Parameters::init() 
    {
        using qi::eol;
        using qi::lexeme;
        using ascii::char_;
        // Rules defining parameter sets for functions
        parameter      %= (Strings::doubleQuotedString |
                           Strings::genericString      |
                           BasicTypes::doubleRule      |
                           BasicTypes::intRule         |
                           BasicTypes::boolRule        | 
                           Expressions::mathExpression |
                           Expressions::bracketedMathExpression | 
                           Expressions::comparisonExpression    |
                           Expressions::bracketedComparisonExpression |
                           Strings::bracketedWords);

        commaParameter %= ',' >> parameter;

        parameters     %= -(parameter >> *commaParameter); // comma-separated

        // a collection of parameters
        // to be used by a function. This could be empty as in
        // () or have arguments, as in (a, b, c);
        parameterList %= parameters;
    }
}