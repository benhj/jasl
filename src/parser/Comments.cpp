#include "Comments.hpp"
#include "Strings.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule< jasl::Comments::Iterator, 
          jasl::Function(), 
          ascii::space_type> jasl::Comments::commentFunc;

namespace jasl {
    void Comments::init() 
    {
        using qi::lexeme;
        using ascii::string;
        commentFunc %= lexeme[string(";;;") | string("//")  | string("#")]
                    >> -Strings::allChars;
    }
}