// Ben Jones 2019

#include "Escapes.hpp"

namespace qi = ::boost::spirit::qi;
qi::rule< jasl::Escapes::Iterator, std::string() > jasl::Escapes::escapes;

namespace jasl {

    struct cp2utf8_f
    {
        template < typename ... > struct result { using type = void; };

        void operator()( std::string & a, UChar32 codepoint ) const
        {
            icu::StringByteSink< std::string > bs( &a );
            icu::UnicodeString::fromUTF32( &codepoint, 1 ).toUTF8( bs );
        }
    };

    void Escapes::init()
    {
        // convenience rules
        static qi::symbols< char const, UChar32 > escaped_character;
        escaped_character.add
        (  "a", 0x07 ) // alert
        (  "b", 0x08 ) // backspace
        (  "f", 0x0c ) // form feed
        (  "n", 0x0a ) // new line
        (  "r", 0x0d ) // carriage return
        (  "t", 0x09 ) // horizontal tab
        (  "v", 0x0b ) // vertical tab
        ( "\"", 0x22 ) // literal quotation mark
        ( "\\", 0x5c ) // literal backslash
        ;

        ::boost::phoenix::function< cp2utf8_f > cp2utf8;
        escapes = '\\' > ( escaped_character[ cp2utf8( ::boost::spirit::_val, ::boost::spirit::_1 ) ]
                                 | ( "x" > qi::uint_parser< UChar32, 16, 2, 2 >()[ cp2utf8( ::boost::spirit::_val, ::boost::spirit::_1 ) ] )
                                 | ( "u" > qi::uint_parser< UChar32, 16, 4, 4 >()[ cp2utf8( ::boost::spirit::_val, ::boost::spirit::_1 ) ] )
                                 | ( "U" > qi::uint_parser< UChar32, 16, 8, 8 >()[ cp2utf8( ::boost::spirit::_val, ::boost::spirit::_1 ) ] )
                                 | qi::uint_parser< UChar32,  8, 1, 3 >()[ cp2utf8( ::boost::spirit::_val, ::boost::spirit::_1 ) ]
                                 );
    }
}