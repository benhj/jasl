#include "SimpleTest.hpp"
#include "CommandInterpretor.hpp"
#include "VarCache.hpp"

namespace ll = lightlang;
using namespace simpletest;

/// reset variable caches after tests
void clearCaches()
{
    decltype(ll::VarCache::intCache)().swap(ll::VarCache::intCache);
    decltype(ll::VarCache::doubleCache)().swap(ll::VarCache::doubleCache);
    decltype(ll::VarCache::boolCache)().swap(ll::VarCache::boolCache);
    decltype(ll::VarCache::stringCache)().swap(ll::VarCache::stringCache);
}

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
    clearCaches();
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
    clearCaches();
}

void testCVarCommandsBasic()
{
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("int i = 1;");
    ci.parseAndInterpretSingleCommand("double j = 1.1;");
    ci.parseAndInterpretSingleCommand("bool k = true;");
    ASSERT_EQUAL(1, ll::VarCache::intCache["i"], "testCVarCommand::i is 1");
    ASSERT_EQUAL(1.1, ll::VarCache::doubleCache["j"], "testCVarCommand::j is 1.1");
    ASSERT_EQUAL(true, ll::VarCache::boolCache["k"], "testCVarCommand::k is true");
    clearCaches();
}

void testCVarCommandsCompound()
{
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("int i = 5 * 2;");
    ASSERT_EQUAL(10, ll::VarCache::intCache["i"], "testCVarCommand::i is 10");
    ci.parseAndInterpretSingleCommand("int j = (5 * i) + (10 * 6);");
    ASSERT_EQUAL(110, ll::VarCache::intCache["j"], "testCVarCommand::j is 110");
    ci.parseAndInterpretSingleCommand("int k = (j * i) * (10 * 0.768);");
    ASSERT_EQUAL(8448, ll::VarCache::intCache["k"], "testCVarCommand::k is 8448");
    ci.parseAndInterpretSingleCommand("double q = k + 0.0;"); // bug -- shouldn't have to add 0.0!
    ci.parseAndInterpretSingleCommand("double L = q / 5.0;");
    ASSERT_EQUAL(1689.6, ll::VarCache::doubleCache["L"], "testCVarCommand::L is 1689.6");

    ci.parseAndInterpretSingleCommand("bool b = (5 > 1) && (8 <= 10);");
    ASSERT_EQUAL(true, ll::VarCache::boolCache["b"], "testCVarCommand::b is true");

    ci.parseAndInterpretSingleCommand("bool c = (5 > 1) && (8 <= 2);");
    ASSERT_EQUAL(false, ll::VarCache::boolCache["c"], "testCVarCommand::c is false");

    ci.parseAndInterpretSingleCommand("bool d = (5 > 1) || (8 <= 2);");
    ASSERT_EQUAL(true, ll::VarCache::boolCache["d"], "testCVarCommand::d is true");

    clearCaches();
}

int main()
{
    testVarCommand();
    testMathCommands();
    testCVarCommandsBasic();
    testCVarCommandsCompound();
    showResults();
}