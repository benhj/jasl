#include "ListManip.hpp"
#include "Strings.hpp"
#include "BasicTypes.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule< jasl::ListManip::Iterator,
          jasl::Function(),
          ::boost::spirit::ascii::space_type> jasl::ListManip::listTokenIndex;
qi::rule< jasl::ListManip::Iterator,
          jasl::Function(),
          ::boost::spirit::ascii::space_type> jasl::ListManip::listGetToken;
qi::rule< jasl::ListManip::Iterator,
          jasl::Function(),
          ::boost::spirit::ascii::space_type> jasl::ListManip::listSetToken;
qi::rule< jasl::ListManip::Iterator,
          jasl::Function(),
          ::boost::spirit::ascii::space_type> jasl::ListManip::listAddToken;

namespace jasl {
    void ListManip::init() 
    {
        using qi::lit;
        using ascii::string;
        // index_of ("hello", [hello there]) -> s;
        listTokenIndex %= string("index_of")
                       >> '('
                       >> (Strings::genericString | Strings::doubleQuotedString)
                       >> lit(",")
                       >> (Strings::bracketedWords | Strings::genericString)
                       >> (')')
                       >> lit("->")
                       >> Strings::genericString 
                       >> ';';

        // get_token(0, [hello there]) -> t;
        listGetToken %= string("get_token")
                     >> '('
                     >> (Strings::genericString | BasicTypes::intRule)
                     >> ','
                     >> (Strings::bracketedWords | Strings::genericString)
                     >> ')'
                     >> lit("->")
                     >> Strings::genericString 
                     >> ';';

        // add_token("token", L);
        listAddToken %= string("add_token")
                     >> '('
                     >> (Strings::genericString | Strings::doubleQuotedString | Strings::bracketedWords)
                     >> ','
                     >> (Strings::genericString)
                     >> ')'
                     >> ';';

        // set_token (0, [hello there], "goodbye") -> t;
        // set_token (0, [hello there], [nested bit]) -> t;
        listSetToken %= string("set_token")
                     >> ('(')
                     >> (Strings::genericString | BasicTypes::intRule)
                     >> (',')
                     >> (Strings::bracketedWords | Strings::genericString)
                     >> (',')
                     >> (Strings::genericString | Strings::doubleQuotedString | Strings::bracketedWords)
                     >> (')')
                     >> lit("->")
                     >> Strings::genericString 
                     >> ';';
    }
}