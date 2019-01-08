#include "BasicTypes.hpp"

namespace qi = ::boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
qi::rule< jasl::BasicTypes::Iterator, double(), ascii::space_type> jasl::BasicTypes::doubleRule;
qi::rule< jasl::BasicTypes::Iterator, int64_t(), ascii::space_type> jasl::BasicTypes::intRule;
qi::rule< jasl::BasicTypes::Iterator, bool(), ascii::space_type> jasl::BasicTypes::boolRule;

namespace jasl {
    void BasicTypes::init() 
    {
        // with thanks to sehe and llonesmix @
        // http://stackoverflow.com/questions/15212671/
        // using-boostspirit-to-parse-multiple-types-of-single-value
        // #comment21459703_15212671
        using qi::long_long;
        using qi::bool_;
        using qi::lit;
        using RealPolicy = qi::strict_real_policies<double>;
        using RealParse = qi::real_parser<double, RealPolicy>;
        RealParse double_with_point;
        doubleRule %= (long_long >> 'f')
                    | (double_with_point >> -lit('f'));
        intRule %= long_long;
        boolRule %= bool_;
    }
}