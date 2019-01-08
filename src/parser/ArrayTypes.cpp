#include "ArrayTypes.hpp"
#include "BasicTypes.hpp"
#include "Strings.hpp"
#include "Expressions.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule< jasl::ArrayTypes::Iterator, std::string(), ascii::space_type> jasl::ArrayTypes::arrayLexeme;
qi::rule< jasl::ArrayTypes::Iterator, std::string(), ascii::space_type> jasl::ArrayTypes::arrayTypes;
qi::rule< jasl::ArrayTypes::Iterator, jasl::Function(), ascii::space_type > jasl::ArrayTypes::array;

namespace jasl {
    void ArrayTypes::init() 
    {
        using qi::lit;
        using qi::lexeme;
        using ascii::string;

        arrayLexeme %= lexeme[string("array")];
        arrayTypes  %= lexeme[string("int") | string("real") | string("byte") | string("string")];

        // simpler array types. Arrays are 'plural' versions of primitives
        // reals 5 -> a;
        array %= lexeme[string("ints")   |
                        string("reals")  |
                        string("bytes")  |
                        string("strings")|
                        string("bools")]
              >> (('('
              >> (Expressions::mathExpression          |
                  Expressions::bracketedMathExpression |
                  BasicTypes::intRule                  |
                  Strings::genericString)
              >> ')') | Strings::genericString)
              >> lit("->")
              >> (Strings::genericString)
              >> ';';
    }
}