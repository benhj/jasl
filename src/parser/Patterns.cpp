#include "Patterns.hpp"
#include "Strings.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule< jasl::Patterns::Iterator,
          jasl::Function(),
          ::boost::spirit::ascii::space_type> jasl::Patterns::pattern;

namespace jasl {
    void Patterns::init() 
    {
        using qi::lexeme;
        using qi::lit;
        using ascii::string;
        pattern %= lexeme[string("regex_eq")    |
                          string("regex_parse") |
                          string("wildcard_eq") |
                          string("wildcard_parse")]
                    >> ('(') 
                    >> (Strings::doubleQuotedString | Strings::genericString) >> ','
                    >> (Strings::doubleQuotedString | Strings::genericString) 
                    >> (')')
                    >> lit("->")
                    >> Strings::genericString
                    >> ';';
    }
}