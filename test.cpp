//
//  test.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "SimpleTest.hpp"
#include "CommandInterpretor.hpp"
#include "VarCache.hpp"

#include <sstream>
#include <vector>

namespace ll = jasl;
using namespace simpletest;

/// reset variable caches after tests
void clearCaches()
{
    decltype(ll::VarCache::bigCache)().swap(ll::VarCache::bigCache);
    decltype(ll::VarCache::script)().swap(ll::VarCache::script);
}

void testRelease()
{
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("integer 5 -> result;");

    // Test that variable result can be found
    ASSERT_UNEQUAL(ll::VarCache::bigCache.find("result"),
                   std::end(ll::VarCache::bigCache),
                   "testRelease::result variable found.");

    // Now release the variable
    ci.parseAndInterpretSingleCommand("release result;");

    // Test that it can no longer be found
    ASSERT_EQUAL(ll::VarCache::bigCache.find("result"),
                 std::end(ll::VarCache::bigCache),
                 "testRelease::result variable not found.");
}

void testMath()
{
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("integer (10 + 20) -> result;");
    ASSERT_EQUAL(30, *ll::VarCache::getInt("result"), "testMath::result is 30");
    ci.parseAndInterpretSingleCommand("integer 2 -> x;");
    ci.parseAndInterpretSingleCommand("integer (result * x) -> result;");
    ASSERT_EQUAL(60, *ll::VarCache::getInt("result"), "testMath::result is 60");
    ci.parseAndInterpretSingleCommand("decimal (result - 2.5) -> resultDouble;");
    ASSERT_EQUAL(57.5, *ll::VarCache::getDouble("resultDouble"), "testMath::resultDouble is 57.5");
    clearCaches();
}

void testVarNewSyntax()
{
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("integer 1->i;");
    ci.parseAndInterpretSingleCommand("decimal 1.1->j;");
    ci.parseAndInterpretSingleCommand("boolean true->k;");
    ASSERT_EQUAL(1, *ll::VarCache::getInt("i"), "testVarNewSyntax::i is 1");
    ASSERT_EQUAL(1.1, *ll::VarCache::getDouble("j"), "testVarNewSyntax::j is 1.1");
    ASSERT_EQUAL(true, *ll::VarCache::getBool("k"), "testVarNewSyntax::k is true");
    clearCaches();
}

void testVarNewSyntaxFromString()
{
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("integer \"1\" -> i;");
    ci.parseAndInterpretSingleCommand("decimal \"1.1\" -> j;");
    ci.parseAndInterpretSingleCommand("string \"2\" -> sint;");
    ci.parseAndInterpretSingleCommand("string \"2.2\" -> sdouble;");
    ci.parseAndInterpretSingleCommand("integer sint -> k;");
    ci.parseAndInterpretSingleCommand("decimal sdouble -> l;");
    ASSERT_EQUAL(1, *ll::VarCache::getInt("i"), "testVarNewSyntaxFromString A");
    ASSERT_EQUAL(1.1, *ll::VarCache::getDouble("j"), "testVarNewSyntaxFromString B");
    ASSERT_EQUAL(2, *ll::VarCache::getInt("k"), "testVarNewSyntaxFromString C");
    ASSERT_EQUAL(2.2, *ll::VarCache::getDouble("l"), "testVarNewSyntaxFromString D");
    clearCaches();
}

void testEchoLiterals()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("echo \"Hello, world!\";", ss);
    ASSERT_EQUAL("Hello, world!", ss.str(), "testEcho: Hello, world!");
    clearCaches();
}

void testEchoNLLiterals()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("echo_nl \"Hello, world!\";", ss);
    ASSERT_EQUAL("Hello, world!\n", ss.str(), "testEchoNL: Hello, world!");
    clearCaches();
}

void testEchoSymbols()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("integer 10 -> b;");
    ci.parseAndInterpretSingleCommand("decimal 1.1 -> c;");
    ci.parseAndInterpretSingleCommand("boolean true -> d;");
    ci.parseAndInterpretSingleCommand("echo b;", ss);
    ci.parseAndInterpretSingleCommand("echo c;", ss);
    ci.parseAndInterpretSingleCommand("echo_nl d;", ss);
    ASSERT_EQUAL("101.11\n", ss.str(), "testEchoSymbols");
    clearCaches();
}

void testEchoPrimitives()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("echo_nl 2;", ss);
    ci.parseAndInterpretSingleCommand("echo_nl 5.67;", ss);
    ci.parseAndInterpretSingleCommand("echo_nl 3.14;", ss);
    ASSERT_EQUAL("2\n5.67\n3.14\n", ss.str(), "testEchoPrimitives");
    clearCaches();
}

void testEchoMath()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("echo_nl (5 + 2) * 5.6;", ss);
    ASSERT_EQUAL("39.2\n", ss.str(), "testEchoMath");
    clearCaches();
}

void testEchoString()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string \"Hello, world!\" -> hello;", ss);
    ci.parseAndInterpretSingleCommand("echo_nl hello;", ss);
    ASSERT_EQUAL("Hello, world!\n", ss.str(), "testEchoString");
    clearCaches();
}

void testStringWithLiteral()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string \"Hello, world!\" -> hello;", ss);
    ASSERT_EQUAL("Hello, world!", *ll::VarCache::getString("hello"), "testStringWithLiteral");
    clearCaches();
}

void testStringWithNumbers()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string 5 -> numberFive;", ss);
    ASSERT_EQUAL("5", *ll::VarCache::getString("numberFive"), "testStringWithNumbers integer");
    ci.parseAndInterpretSingleCommand("string 5.5 -> numberDecimal;", ss);
    ASSERT_EQUAL("5.5", *ll::VarCache::getString("numberDecimal"), "testStringWithNumbers decimal");
    clearCaches();
}

void testStringWithMath()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string (5 + 2) * 5.6 -> numberMath;", ss);
    ci.parseAndInterpretSingleCommand("string (5 + 2) * 5 -> numberMathB;", ss);
    ASSERT_EQUAL("39.2", *ll::VarCache::getString("numberMath"), "testStringWithMath A");
    ASSERT_EQUAL("35", *ll::VarCache::getString("numberMathB"), "testStringWithMath B");
    clearCaches();
}

void testStringFromList()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string [hello there] -> listString;", ss);
    ci.parseAndInterpretSingleCommand("list [another list] -> listB;");
    ci.parseAndInterpretSingleCommand("string listB -> listStringB;", ss);
    ASSERT_EQUAL("hello there", *ll::VarCache::getString("listString"), "testStringFromList A");
    ASSERT_EQUAL("another list", *ll::VarCache::getString("listStringB"), "testStringFromList B");
}

void testAppendLiteral()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string \"Hello\" -> s;", ss);
    ci.parseAndInterpretSingleCommand("append (s, \", world!\") -> s;", ss);
    ASSERT_EQUAL("Hello, world!", *ll::VarCache::getString("s"), "testAppendLiteral");
    clearCaches();
}

void testAppendString()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string \"Hello\" -> s;", ss);
    ci.parseAndInterpretSingleCommand("string \", world!\" -> b;", ss);
    ci.parseAndInterpretSingleCommand("append (s, b) -> s;", ss);
    ASSERT_EQUAL("Hello, world!", *ll::VarCache::getString("s"), "testAppendString");
    clearCaches();
}

void testReverseString()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string \"Hello\" -> s;", ss);
    ci.parseAndInterpretSingleCommand("string_reverse s;", ss);
    ASSERT_EQUAL("olleH", *ll::VarCache::getString("s"), "testReverseString");
    clearCaches();
}

void testStringLength()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string_length \"Hello\" -> s;", ss);
    ci.parseAndInterpretSingleCommand("string \"Hello, world!\" -> sb;", ss);
    ci.parseAndInterpretSingleCommand("string_length sb -> sc;", ss);
    ASSERT_EQUAL(5, *ll::VarCache::getInt("s"), "testStringLength A");
    ASSERT_EQUAL(13, *ll::VarCache::getInt("sc"), "testStringLength B");
    clearCaches();
}

void testIfCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("if(1 < 2) { echo \"Hello, world!\"; integer 5 -> x; }", ss);
    ASSERT_EQUAL("Hello, world!", ss.str(), "testIfCommand: Hello, world!");
    ASSERT_EQUAL(5, *ll::VarCache::getInt("x"), "testIfCommand: x is 5");
    clearCaches();
}

void testRepeatCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string const commands("integer 0 -> x; repeat 5 times { echo \"Hello\"; integer (x + 1) -> x; }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        (void)ci.interpretFunc(f, ss);
    }
    ASSERT_EQUAL("HelloHelloHelloHelloHello", ss.str(), "testRepeatCommand::repeat hello");
    ASSERT_EQUAL(5, *ll::VarCache::getInt("x"), "testRepeatCommand::x is 5");
    clearCaches();
}

void testWhileCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string const commands("integer 5 -> x; while(x > 0) { integer (x - 1) -> x; }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        (void)ci.interpretFunc(f, ss);
    }
    ASSERT_EQUAL(0, *ll::VarCache::getInt("x"), "testWhileCommand::x is 0");
    clearCaches();
}

void testBlockCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string commands("integer 0 -> x; block foo { echo \"Hello\"; integer (x + 1) -> x; }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        (void)ci.interpretFunc(f, ss);
    }
    ASSERT_EQUAL("Hello", ss.str(), "testBlockCommand::print hello");
    ASSERT_EQUAL(1, *ll::VarCache::getInt("x"), "testBlockCommand::x is 1");
    clearCaches();
}

void testStartCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string const command("start { echo \"Hello\"; integer 21 -> x; }");
    ci.parseAndInterpretSingleCommand(command, ss);
    ASSERT_EQUAL("Hello", ss.str(), "testStartCommand::print hello");
    ASSERT_EQUAL(21, *ll::VarCache::getInt("x"), "testStartCommand::x is 21");
    clearCaches();
}

void testCallCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string commands("start { echo \"Starting..\"; call foo; call bar; integer (x - 5) -> x;}");
    commands.append("block bar { echo \"..and Goodbye!\"; integer (x + 1) -> x; }");
    commands.append("block foo { echo \"Hello\"; integer 20 -> x; }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        if(f.name == "start") {
            (void)ci.interpretFunc(f, ss);
            break;
        }
    }
    ASSERT_EQUAL("Starting..Hello..and Goodbye!", ss.str(), "testCallCommand::print-outs");
    ASSERT_EQUAL(16, *ll::VarCache::getInt("x"), "testStartCommand::x is 16");
    clearCaches();
}

void testArgsCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ll::VarCache::args.push_back("argument1");
    ll::VarCache::args.push_back("argument2");
    ci.parseAndInterpretSingleCommand("args 0 -> arga;", ss);
    ci.parseAndInterpretSingleCommand("args 1 -> argb;", ss);
    ASSERT_EQUAL("argument1", *ll::VarCache::getString("arga"), "testArgsCommand A");
    ASSERT_EQUAL("argument2", *ll::VarCache::getString("argb"), "testArgsCommand B");
    clearCaches();
}

void testList()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("list [hello there] -> s;", ss);

    std::string tokenA;
    ll::VarExtractor::tryAnyCast(tokenA, ll::VarCache::getListToken("s", 0));
    ASSERT_EQUAL("hello", tokenA, "testList tokenA");

    std::string tokenB;
    ll::VarExtractor::tryAnyCast(tokenB, ll::VarCache::getListToken("s", 1));
    ASSERT_EQUAL("there", tokenB, "testList tokenB");

    clearCaches();
}

void testListGetTokenRaw()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("get_token (0, [hello there]) -> s;", ss);
    ci.parseAndInterpretSingleCommand("integer 1 -> i;");
    ci.parseAndInterpretSingleCommand("get_token (i, [hello there]) -> b;", ss);
    ASSERT_EQUAL("hello", *ll::VarCache::getString("s"), "testListGetTokenRaw A");
    ASSERT_EQUAL("there", *ll::VarCache::getString("b"), "testListGetTokenRaw B");
    clearCaches();
}

void testListGetTokenSymbol()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("list [hello there] -> lst;", ss);
    ci.parseAndInterpretSingleCommand("get_token (1, lst) -> s;", ss);
    ci.parseAndInterpretSingleCommand("integer 0 -> i;");
    ci.parseAndInterpretSingleCommand("get_token (i, lst) -> b;", ss);
    ASSERT_EQUAL("there", *ll::VarCache::getString("s"), "testListGetTokenSymbol A");
    ASSERT_EQUAL("hello", *ll::VarCache::getString("b"), "testListGetTokenSymbol B");
    clearCaches();
}

void testListSetTokenRaw()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("set_token (0, [hello there], \"goodbye\") -> s;", ss);
    std::string tokenA;
    ll::VarExtractor::tryAnyCast(tokenA, ll::VarCache::getListToken("s", 0));
    ASSERT_EQUAL("goodbye", tokenA, "testListSetTokenRaw A");

    ci.parseAndInterpretSingleCommand("string \"yes\" -> b;", ss);
    ci.parseAndInterpretSingleCommand("set_token (1, [hello there], b) -> s;", ss);
    std::string tokenB;
    ll::VarExtractor::tryAnyCast(tokenB, ll::VarCache::getListToken("s", 1));
    ASSERT_EQUAL("yes", tokenB, "testListSetTokenRaw B");

    clearCaches();
}

void testListSetTokenSymbol()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("list [hello there] -> la;", ss);
    ci.parseAndInterpretSingleCommand("list [what are you doing] -> lb;", ss);
    ci.parseAndInterpretSingleCommand("list [hello there] -> la;", ss);
    ci.parseAndInterpretSingleCommand("integer 3 -> i;", ss);
    ci.parseAndInterpretSingleCommand("integer 2 -> j;", ss);
    ci.parseAndInterpretSingleCommand("set_token (0, la, \"goodbye\") -> sa;", ss);
    ci.parseAndInterpretSingleCommand("set_token (i, lb, \"indeed\") -> sb;", ss);

    std::string tokenA;
    ll::VarExtractor::tryAnyCast(tokenA, ll::VarCache::getListToken("sa", 0));
    ASSERT_EQUAL("goodbye", tokenA, "testListSetTokenSymbol A");
    std::string tokenB;
    ll::VarExtractor::tryAnyCast(tokenB, ll::VarCache::getListToken("sb", 3));
    ASSERT_EQUAL("indeed", tokenB, "testListSetTokenSymbol B");
    std::string tokenC;
    ll::VarExtractor::tryAnyCast(tokenC, ll::VarCache::getListToken("lb", 2));
    ASSERT_EQUAL("you", tokenC, "testListSetTokenSymbol C");
    ci.parseAndInterpretSingleCommand("set_token (j, lb, \"no\") -> lb;", ss);
    std::string tokenD;
    ll::VarExtractor::tryAnyCast(tokenD, ll::VarCache::getListToken("lb", 2));
    ASSERT_EQUAL("no", tokenD, "testListSetTokenSymbol D");

    clearCaches();
}

void testListTokenIndex()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("index_of (\"are\", [hello there what are you doing]) -> i;", ss);
    ci.parseAndInterpretSingleCommand("string \"you\" -> token;");
    ci.parseAndInterpretSingleCommand("index_of (token, [hello there what are you doing]) -> j;", ss);
    ci.parseAndInterpretSingleCommand("list [hello there what are you doing] -> L;");
    ci.parseAndInterpretSingleCommand("index_of (token, L) -> p;", ss);
    ci.parseAndInterpretSingleCommand("index_of (token, [hello there what are you doing]) -> q;", ss);
    ASSERT_EQUAL(3, *ll::VarCache::getInt("i"), "index A");
    ASSERT_EQUAL(4, *ll::VarCache::getInt("j"), "index B");
    ASSERT_EQUAL(4, *ll::VarCache::getInt("p"), "index C");
    ASSERT_EQUAL(4, *ll::VarCache::getInt("q"), "index D");
    clearCaches();
}

int main()
{
    testRelease();
    testMath();
    testVarNewSyntax();
    testVarNewSyntaxFromString();
    testEchoLiterals();
    testEchoNLLiterals();
    testEchoSymbols();
    testEchoPrimitives(); 
    testEchoMath();   
    testEchoString();  
    testStringWithLiteral();
    testStringWithNumbers();
    testStringWithMath();
    testStringFromList();
    testAppendLiteral();
    testAppendString();
    testReverseString();
    testStringLength();
    testIfCommand();
    testRepeatCommand();
    testWhileCommand();
    testBlockCommand();
    testStartCommand();
    testCallCommand();
    testArgsCommand();
    testList();
    testListTokenIndex();
    testListGetTokenRaw();
    testListGetTokenSymbol();
    testListSetTokenRaw();
    testListSetTokenSymbol();
    showResults();
    return 0;
}
