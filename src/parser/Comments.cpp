#include "Comments.hpp"
#include "Strings.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule< jasl::Comments::Iterator, std::string(), ascii::space_type > jasl::Comments::commentSlash;
qi::rule< jasl::Comments::Iterator, std::string(), ascii::space_type > jasl::Comments::commentColons;
qi::rule< jasl::Comments::Iterator, std::string(), ascii::space_type > jasl::Comments::commentPound;
qi::rule< jasl::Comments::Iterator, jasl::Function(), ascii::space_type> jasl::Comments::commentFunc;

namespace jasl {
    void Comments::init() 
    {
        using qi::lexeme;
        using ascii::string;
        commentSlash   %= string("//");
        commentColons  %= string(";;;");
        commentPound   %= string("#");
        commentFunc    %= (commentColons | commentSlash | commentPound) >> -Strings::allChars;
    }
}