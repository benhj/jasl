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
qi::rule< jasl::Strings::Iterator, 
          std::string(),
          ascii::space_type> jasl::Strings::allChars;
qi::rule< jasl::Strings::Iterator,
          std::string(), 
          ascii::space_type> jasl::Strings::word;
qi::rule< jasl::Strings::Iterator,
          ValueArray(),
          ascii::space_type> jasl::Strings::words;
qi::rule< jasl::Strings::Iterator,
          Value(),
          ascii::space_type> jasl::Strings::bracketedWords;

namespace jasl {
    void Strings::init() 
    {
        using qi::eol;
        using qi::lexeme;
        using ascii::char_;
        genericString       %= lexeme[+(char_("a-zA-Z_"))];
        quotedString        %= lexeme['\''>> *( +( char_ - ( '"' | eol | '\\' ) ) | Escapes::escapes ) >> '\''];
        doubleQuotedString  %= lexeme['"' >> *( +( char_ - ( '"' | eol | '\\' ) ) | Escapes::escapes ) >> '"'];
        allChars            %= lexeme[+(char_ - '\n')];
        word                %= lexeme[+(char_ - ' ' - ',' - '[' - ']' - '(' - ')')];
        words               %= *(word | bracketedWords); // zero or more words
        bracketedWords      %= '[' >> words >> ']';
    }
}
