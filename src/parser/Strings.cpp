#include "Strings.hpp"
#include "Escapes.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule< jasl::Strings::Iterator,
          std::string(),
          ascii::space_type> jasl::Strings::genericString;
qi::rule< jasl::Strings::Iterator,
          jasl::LiteralString(),
          ascii::space_type> jasl::Strings::quotedString;
qi::rule< jasl::Strings::Iterator,
          jasl::LiteralString(),
          ascii::space_type> jasl::Strings::doubleQuotedString;

namespace jasl {
    void Strings::init() 
    {
        using qi::eol;
        using qi::lexeme;
        using ascii::char_;
        genericString       %= lexeme[+(char_("a-zA-Z_"))];
        quotedString        %= lexeme['\''>> *( +( char_ - ( '"' | eol | '\\' ) ) | Escapes::escapes ) >> '\''];
        doubleQuotedString  %= lexeme['"' >> *( +( char_ - ( '"' | eol | '\\' ) ) | Escapes::escapes ) >> '"'];
    }
}