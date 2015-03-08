#include "SimpleTest.hpp"
#include "CommandInterpretor.hpp"
#include "VarCache.hpp"

namespace ll = lightlang;
using namespace simpletest;

void testVarCommand()
{
    ll::CommandInterpretor ci;
    ci.parseCommandString("var(int, a, 5);");
    ci.parseCommandString("var(int, b, 10);");
    ci.parseCommandString("var(double, c, 1.1);");
    ci.parseCommandString("var(bool, d, true);");
    ci.parseCommandString("var(bool, e, false);");
    ASSERT_UNEQUAL(ll::VarCache::intCache.find("a"), ll::VarCache::intCache.end(), "found integer a");
    ASSERT_EQUAL(5, ll::VarCache::intCache["a"], "integer a is 5");
    ASSERT_UNEQUAL(ll::VarCache::intCache.find("b"), ll::VarCache::intCache.end(), "found integer b");
    ASSERT_EQUAL(10, ll::VarCache::intCache["b"], "integer b is 5");        
    ASSERT_UNEQUAL(ll::VarCache::doubleCache.find("c"), ll::VarCache::doubleCache.end(), "found double c");
    ASSERT_EQUAL(1.1, ll::VarCache::doubleCache["c"], "double c is 1.1");
    ASSERT_UNEQUAL(ll::VarCache::boolCache.find("d"), ll::VarCache::boolCache.end(), "found bool d");
    ASSERT_EQUAL(true, ll::VarCache::boolCache["d"], "bool d is true");   
    ASSERT_UNEQUAL(ll::VarCache::boolCache.find("e"), ll::VarCache::boolCache.end(), "found bool e");
    ASSERT_EQUAL(false, ll::VarCache::boolCache["e"], "bool e is false");   
}

int main()
{
    testVarCommand();
    showResults();
}