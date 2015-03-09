#include "SimpleTest.hpp"
#include "CommandInterpretor.hpp"
#include "VarCache.hpp"

namespace ll = lightlang;
using namespace simpletest;

void testVarCommand()
{
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("var(int, a, 5);");
    ci.parseAndInterpretSingleCommand("var(int, b, 10);");
    ci.parseAndInterpretSingleCommand("var(double, c, 1.1);");
    ci.parseAndInterpretSingleCommand("var(bool, d, true);");
    ci.parseAndInterpretSingleCommand("var(bool, e, false);");
    ASSERT_UNEQUAL(ll::VarCache::intCache.find("a"), ll::VarCache::intCache.end(), "testVarCommand::found integer a");
    ASSERT_EQUAL(5, ll::VarCache::intCache["a"], "testVarCommand::integer a is 5");
    ASSERT_UNEQUAL(ll::VarCache::intCache.find("b"), ll::VarCache::intCache.end(), "testVarCommand::found integer b");
    ASSERT_EQUAL(10, ll::VarCache::intCache["b"], "testVarCommand::integer b is 5");        
    ASSERT_UNEQUAL(ll::VarCache::doubleCache.find("c"), ll::VarCache::doubleCache.end(), "testVarCommand::found double c");
    ASSERT_EQUAL(1.1, ll::VarCache::doubleCache["c"], "testVarCommand::double c is 1.1");
    ASSERT_UNEQUAL(ll::VarCache::boolCache.find("d"), ll::VarCache::boolCache.end(), "testVarCommand::found bool d");
    ASSERT_EQUAL(true, ll::VarCache::boolCache["d"], "testVarCommand::bool d is true");   
    ASSERT_UNEQUAL(ll::VarCache::boolCache.find("e"), ll::VarCache::boolCache.end(), "testVarCommand::found bool e");
    ASSERT_EQUAL(false, ll::VarCache::boolCache["e"], "testVarCommand::bool e is false");   
}

void testMathCommands()
{
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("m_add(10, 20, result);");
    ASSERT_UNEQUAL(ll::VarCache::intCache.find("result"), 
                   ll::VarCache::intCache.end(), 
                   "testMathCommands::found integer result");
    ASSERT_EQUAL(30, ll::VarCache::intCache["result"], "testMathCommands::result is 30");
    ci.parseAndInterpretSingleCommand("var(int, x, 2);");
    ci.parseAndInterpretSingleCommand("m_mult(result, x, result);");
    ASSERT_EQUAL(60, ll::VarCache::intCache["result"], "testMathCommands::result is 60");
    ci.parseAndInterpretSingleCommand("m_sub(result, 2.5, resultDouble);");
    ASSERT_UNEQUAL(ll::VarCache::doubleCache.find("resultDouble"), 
                   ll::VarCache::doubleCache.end(), 
                   "testMathCommands::found float resultDouble");
    ASSERT_EQUAL(57.5, ll::VarCache::doubleCache["resultDouble"], "testMathCommands::resultDouble is 57.5");
}

int main()
{
    testVarCommand();
    testMathCommands();
    showResults();
}