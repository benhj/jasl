#include "StringManip.hpp"
#include "Strings.hpp"
#include "BasicTypes.hpp"
#include "Expressions.hpp"
#include "Parameters.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule<jasl::StringManip::Iterator, jasl::Function(), ascii::space_type> jasl::StringManip::appendRule;
qi::rule<jasl::StringManip::Iterator, jasl::Function(), ascii::space_type> jasl::StringManip::reverseRule;
qi::rule<jasl::StringManip::Iterator, jasl::Function(), ascii::space_type> jasl::StringManip::concatRule;

namespace jasl {
    void StringManip::init() 
    {
        using qi::lit;
        using ascii::string;
        // appends to end of a string, s  
        // append (s, "hello") -> result;
        appendRule %= string("append")
                   >> ('(')
                   >> (Strings::doubleQuotedString | Strings::genericString)
                   >> ','
                   >> (Strings::doubleQuotedString | Strings::genericString | BasicTypes::doubleRule | BasicTypes::intRule | BasicTypes::boolRule | 
                       Expressions::mathExpression | Expressions::bracketedMathExpression | 
                       Expressions::comparisonExpression | Expressions::bracketedComparisonExpression)
                   >> ')'
                   >> lit("->")
                   >> Strings::genericString
                   >> ';';

        // string_reverse name; 
        reverseRule %= string("string_reverse")
                    >> (Strings::doubleQuotedString | Strings::genericString)
                    >> ';';

        // concatenate strings. 
        concatRule %= string("concat")
                   >> (Parameters::parameterList) >> lit("->")
                   >> Strings::genericString 
                   >> ';';
    }
}