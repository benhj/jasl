//
//  test.cpp
//  jasl
//
//  Created by Ben Jones 
//  Copyright (c) 2015 Ben Jones. All rights reserved.
//

#include "../SimpleTest.hpp"
#include "../CommandInterpretor.hpp"
#include "../GlobalCache.hpp"
#include "../SharedVarCache.hpp"

#include <sstream>
#include <vector>

namespace ll = jasl;
using namespace simpletest;

void testTypes()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("int 5 -> a;", cache);
    ci.parseAndInterpretSingleCommand("real 1.1 -> b;", cache);
    ci.parseAndInterpretSingleCommand("bool true -> c;", cache);
    ci.parseAndInterpretSingleCommand("string \"hello\" -> d;", cache);
    ci.parseAndInterpretSingleCommand("list [some big list] -> e;", cache);
    ci.parseAndInterpretSingleCommand("array:int(5) -> f;", cache);
    ci.parseAndInterpretSingleCommand("array:real(5) -> g;", cache);
    ci.parseAndInterpretSingleCommand("type a -> atype;", cache);
    ci.parseAndInterpretSingleCommand("type b -> btype;", cache);
    ci.parseAndInterpretSingleCommand("type c -> ctype;", cache);
    ci.parseAndInterpretSingleCommand("type d -> dtype;", cache);
    ci.parseAndInterpretSingleCommand("type e -> etype;", cache);
    ci.parseAndInterpretSingleCommand("type f -> ftype;", cache);
    ci.parseAndInterpretSingleCommand("type g -> gtype;", cache);
    ASSERT_EQUAL("int", *cache->getString("atype"), "testType::a is int");
    ASSERT_EQUAL("real", *cache->getString("btype"), "testType::b is real");
    ASSERT_EQUAL("bool", *cache->getString("ctype"), "testType::c is bool");
    ASSERT_EQUAL("string", *cache->getString("dtype"), "testType::d is string");
    ASSERT_EQUAL("list", *cache->getString("etype"), "testType::e is list");
    ASSERT_EQUAL("array:int", *cache->getString("ftype"), "testType::f is array:int");
    ASSERT_EQUAL("array:real", *cache->getString("gtype"), "testType::g is array:real");
}

void testMath()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("int (10 + 20) -> result;", cache);
    ASSERT_EQUAL(30, *cache->getInt("result"), "testMath::result is 30");
    ci.parseAndInterpretSingleCommand("int 2 -> x;", cache);
    ci.parseAndInterpretSingleCommand("int (result * x) -> result;", cache);
    ASSERT_EQUAL(60, *cache->getInt("result"), "testMath::result is 60");
    ci.parseAndInterpretSingleCommand("real (result - 2.5) -> resultDouble;", cache);
    ASSERT_EQUAL(57.5, *cache->getDouble("resultDouble"), "testMath::resultDouble is 57.5");
}

void testVarNewSyntax()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("int 1->i;", cache);
    ci.parseAndInterpretSingleCommand("real 1.1->j;", cache);
    ci.parseAndInterpretSingleCommand("bool true->k;", cache);
    ASSERT_EQUAL(1, *cache->getInt("i"), "testVarNewSyntax::i is 1");
    ASSERT_EQUAL(1.1, *cache->getDouble("j"), "testVarNewSyntax::j is 1.1");
    ASSERT_EQUAL(true, *cache->getBool("k"), "testVarNewSyntax::k is true");
}

void testVarNewSyntaxFromString()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("int \"1\" -> i;", cache);
    ci.parseAndInterpretSingleCommand("real \"1.1\" -> j;", cache);
    ci.parseAndInterpretSingleCommand("string \"2\" -> sint;", cache);
    ci.parseAndInterpretSingleCommand("string \"2.2\" -> sdouble;", cache);
    ci.parseAndInterpretSingleCommand("int sint -> k;", cache);
    ci.parseAndInterpretSingleCommand("real sdouble -> l;", cache);
    ASSERT_EQUAL(1, *cache->getInt("i"), "testVarNewSyntaxFromString A");
    ASSERT_EQUAL(1.1, *cache->getDouble("j"), "testVarNewSyntaxFromString B");
    ASSERT_EQUAL(2, *cache->getInt("k"), "testVarNewSyntaxFromString C");
    ASSERT_EQUAL(2.2, *cache->getDouble("l"), "testVarNewSyntaxFromString D");
}

void testprLiterals()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("pr \"Hello, world!\";", cache, ss);
    ASSERT_EQUAL("Hello, world!", ss.str(), "testpr: Hello, world!");
}

void testprNLLiterals()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("prn \"Hello, world!\";", cache, ss);
    ASSERT_EQUAL("Hello, world!\n", ss.str(), "testprNL: Hello, world!");
}

void testprSymbols()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("int 10 -> b;", cache);
    ci.parseAndInterpretSingleCommand("real 1.1 -> c;", cache);
    ci.parseAndInterpretSingleCommand("bool true -> d;", cache);
    ci.parseAndInterpretSingleCommand("pr b;", cache, ss);
    ci.parseAndInterpretSingleCommand("pr c;", cache, ss);
    ci.parseAndInterpretSingleCommand("prn d;", cache, ss);
    ASSERT_EQUAL("101.11\n", ss.str(), "testprSymbols");
}

void testprPrimitives()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("prn 2;", cache, ss);
    ci.parseAndInterpretSingleCommand("prn 5.67;", cache, ss);
    ci.parseAndInterpretSingleCommand("prn 3.14;", cache, ss);
    ASSERT_EQUAL("2\n5.67\n3.14\n", ss.str(), "testprPrimitives");
}

void testprMath()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("prn (5 + 2) * 5.6;", cache, ss);
    ASSERT_EQUAL("39.2\n", ss.str(), "testprMath");
}

void testprString()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string \"Hello, world!\" -> hello;", cache, ss);
    ci.parseAndInterpretSingleCommand("prn hello;", cache, ss);
    ASSERT_EQUAL("Hello, world!\n", ss.str(), "testprString");
}

void testStringWithLiteral()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string \"Hello, world!\" -> hello;", cache, ss);
    ASSERT_EQUAL("Hello, world!", *cache->getString("hello"), "testStringWithLiteral");
}

void testStringWithNumbers()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string 5 -> numberFive;", cache, ss);
    ASSERT_EQUAL("5", *cache->getString("numberFive"), "testStringWithNumbers int");
    ci.parseAndInterpretSingleCommand("string 5.5 -> numberDecimal;", cache, ss);
    ASSERT_EQUAL("5.5", *cache->getString("numberDecimal"), "testStringWithNumbers real");
}

void testStringWithMath()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string (5 + 2) * 5.6 -> numberMath;", cache, ss);
    ci.parseAndInterpretSingleCommand("string (5 + 2) * 5 -> numberMathB;", cache, ss);
    ASSERT_EQUAL("39.2", *cache->getString("numberMath"), "testStringWithMath A");
    ASSERT_EQUAL("35", *cache->getString("numberMathB"), "testStringWithMath B");
}

void testStringFromList()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string [hello there] -> listString;", cache, ss);
    ci.parseAndInterpretSingleCommand("list [another list] -> listB;", cache);
    ci.parseAndInterpretSingleCommand("string listB -> listStringB;", cache, ss);
    ASSERT_EQUAL("hello there", *cache->getString("listString"), "testStringFromList A");
    ASSERT_EQUAL("another list", *cache->getString("listStringB"), "testStringFromList B");
}

void testAppendLiteral()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string \"Hello\" -> s;", cache, ss);
    ci.parseAndInterpretSingleCommand("append (s, \", world!\") -> s;", cache, ss);
    ASSERT_EQUAL("Hello, world!", *cache->getString("s"), "testAppendLiteral");
}

void testAppendString()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string \"Hello\" -> s;", cache, ss);
    ci.parseAndInterpretSingleCommand("string \", world!\" -> b;", cache, ss);
    ci.parseAndInterpretSingleCommand("append (s, b) -> s;", cache, ss);
    ASSERT_EQUAL("Hello, world!", *cache->getString("s"), "testAppendString");
}

void testReverseString()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string \"Hello\" -> s;", cache, ss);
    ci.parseAndInterpretSingleCommand("string_reverse s;", cache, ss);
    ASSERT_EQUAL("olleH", *cache->getString("s"), "testReverseString");
}

void testStringLength()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string_length \"Hello\" -> s;", cache, ss);
    ci.parseAndInterpretSingleCommand("string \"Hello, world!\" -> sb;", cache, ss);
    ci.parseAndInterpretSingleCommand("string_length sb -> sc;", cache, ss);
    ASSERT_EQUAL(5, *cache->getInt("s"), "testStringLength A");
    ASSERT_EQUAL(13, *cache->getInt("sc"), "testStringLength B");
}

void testIfCommand()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("if(1 < 2) { pr \"Hello, world!\"; int 5 -> x; }", cache, ss);
    ASSERT_EQUAL("Hello, world!", ss.str(), "testIfCommand: Hello, world!");
    ASSERT_EQUAL(5, *cache->getInt("x"), "testIfCommand: x is 5");
}

void testRepeatCommand()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string const commands("int 0 -> x; repeat 5 times { pr \"Hello\"; int (x + 1) -> x; }");
    auto functions = ci.parseStringCollection(commands, cache);
    for(auto &f : functions) {
        (void)ci.interpretFunc(f, cache, ss);
    }
    ASSERT_EQUAL("HelloHelloHelloHelloHello", ss.str(), "testRepeatCommand::repeat hello");
    ASSERT_EQUAL(5, *cache->getInt("x"), "testRepeatCommand::x is 5");
}

void testWhileCommand()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string const commands("int 5 -> x; while(x > 0) { int (x - 1) -> x; }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        (void)ci.interpretFunc(f, cache, ss);
    }
    ASSERT_EQUAL(0, *cache->getInt("x"), "testWhileCommand::x is 0");
}

void testStartCommand()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string const command("start { pr \"Hello\"; int 21 -> x; }");
    ci.parseAndInterpretSingleCommand(command, cache, ss);
    ASSERT_EQUAL("Hello", ss.str(), "testStartCommand::print hello");
    ASSERT_EQUAL(21, *cache->getInt("x"), "testStartCommand::x is 21");
}

void testCallReturnableCommand()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string commands("start { pr \"Starting..\"; int 0 -> x; call foo (x) -> x; call bar (x) -> x; int (x - 5) -> x;}");
    commands.append("fn:int bar(x) -> x { pr \"..and Goodbye!\"; int (x + 1) -> x; }");
    commands.append("fn:int foo(x) -> x { pr \"Hello\"; int 20 -> x;}");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        if(f.name == "start") {
            (void)ci.interpretFunc(f, cache, ss);
            break;
        }
    }
    ASSERT_EQUAL("Starting..Hello..and Goodbye!", ss.str(), "testCallReturnableCommand::print-outs");
    ASSERT_EQUAL(16, *cache->getInt("x"), "testCallReturnableCommand::x is 16");
}

void testCallBlockCommand()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string commands("start { pr \"Starting..\"; call foo (); call bar ();}");
    commands.append("block bar() { pr \"..and Goodbye!\"; }");
    commands.append("block foo() { pr \"Hello\"; }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        if(f.name == "start") {
            (void)ci.interpretFunc(f, cache, ss);
            break;
        }
    }
    ASSERT_EQUAL("Starting..Hello..and Goodbye!", ss.str(), "testCallBlockCommand::print-outs");
}

void testCallFunctionNil()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string commands("start { pr \"Starting..\"; call foo (); call bar ();}");
    commands.append("fn:nil bar() { pr \"..and Goodbye!\"; }");
    commands.append("fn:nil foo() { pr \"Hello\"; }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        if(f.name == "start") {
            (void)ci.interpretFunc(f, cache, ss);
            break;
        }
    }
    ASSERT_EQUAL("Starting..Hello..and Goodbye!", ss.str(), "testCallFunctionNil::print-outs");
}

void testArgsCommand()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ll::GlobalCache::args.push_back("argument1");
    ll::GlobalCache::args.push_back("argument2");
    ci.parseAndInterpretSingleCommand("args 0 -> arga;", cache, ss);
    ci.parseAndInterpretSingleCommand("args 1 -> argb;", cache, ss);
    ASSERT_EQUAL("argument1", *cache->getString("arga"), "testArgsCommand A");
    ASSERT_EQUAL("argument2", *cache->getString("argb"), "testArgsCommand B");
}

void testList()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("list [hello there] -> s;", cache, ss);

    std::string tokenA;
    ll::VarExtractor::tryAnyCast(tokenA, cache->getListToken("s", 0));
    ASSERT_EQUAL("hello", tokenA, "testList tokenA");

    std::string tokenB;
    ll::VarExtractor::tryAnyCast(tokenB, cache->getListToken("s", 1));
    ASSERT_EQUAL("there", tokenB, "testList tokenB");

}

void testListGetTokenRaw()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("get_token (0, [hello there]) -> s;", cache, ss);
    ci.parseAndInterpretSingleCommand("int 1 -> i;", cache);
    ci.parseAndInterpretSingleCommand("get_token (i, [hello there]) -> b;", cache, ss);
    ASSERT_EQUAL("hello", *cache->getString("s"), "testListGetTokenRaw A");
    ASSERT_EQUAL("there", *cache->getString("b"), "testListGetTokenRaw B");
}

void testListGetTokenSymbol()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("list [hello there] -> lst;", cache, ss);
    ci.parseAndInterpretSingleCommand("get_token (1, lst) -> s;", cache, ss);
    ci.parseAndInterpretSingleCommand("int 0 -> i;", cache);
    ci.parseAndInterpretSingleCommand("get_token (i, lst) -> b;", cache, ss);
    ASSERT_EQUAL("there", *cache->getString("s"), "testListGetTokenSymbol A");
    ASSERT_EQUAL("hello", *cache->getString("b"), "testListGetTokenSymbol B");
}

void testListSetTokenRaw()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("set_token (0, [hello there], \"goodbye\") -> s;", cache, ss);
    std::string tokenA;
    ll::VarExtractor::tryAnyCast(tokenA, cache->getListToken("s", 0));
    ASSERT_EQUAL("goodbye", tokenA, "testListSetTokenRaw A");

    ci.parseAndInterpretSingleCommand("string \"yes\" -> b;", cache, ss);
    ci.parseAndInterpretSingleCommand("set_token (1, [hello there], b) -> s;", cache, ss);
    std::string tokenB;
    ll::VarExtractor::tryAnyCast(tokenB, cache->getListToken("s", 1));
    ASSERT_EQUAL("yes", tokenB, "testListSetTokenRaw B");
}

void testListSetTokenSymbol()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("list [hello there] -> la;", cache, ss);
    ci.parseAndInterpretSingleCommand("list [what are you doing] -> lb;", cache, ss);
    ci.parseAndInterpretSingleCommand("list [hello there] -> la;", cache, ss);
    ci.parseAndInterpretSingleCommand("int 3 -> i;", cache, ss);
    ci.parseAndInterpretSingleCommand("int 2 -> j;", cache, ss);
    ci.parseAndInterpretSingleCommand("set_token (0, la, \"goodbye\") -> sa;", cache, ss);
    ci.parseAndInterpretSingleCommand("set_token (i, lb, \"indeed\") -> sb;", cache, ss);

    std::string tokenA;
    ll::VarExtractor::tryAnyCast(tokenA, cache->getListToken("sa", 0));
    ASSERT_EQUAL("goodbye", tokenA, "testListSetTokenSymbol A");
    std::string tokenB;
    ll::VarExtractor::tryAnyCast(tokenB, cache->getListToken("sb", 3));
    ASSERT_EQUAL("indeed", tokenB, "testListSetTokenSymbol B");
    std::string tokenC;
    ll::VarExtractor::tryAnyCast(tokenC, cache->getListToken("lb", 2));
    ASSERT_EQUAL("you", tokenC, "testListSetTokenSymbol C");
    ci.parseAndInterpretSingleCommand("set_token (j, lb, \"no\") -> lb;", cache, ss);
    std::string tokenD;
    ll::VarExtractor::tryAnyCast(tokenD,  cache->getListToken("lb", 2));
    ASSERT_EQUAL("no", tokenD, "testListSetTokenSymbol D");

}

void testListAddToken()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("list [hello there] -> la;", cache);
    ci.parseAndInterpretSingleCommand("add_token(\"again\", la);", cache);
    ci.parseAndInterpretSingleCommand("pr la;", cache, ss);
    ASSERT_EQUAL("[hello there again]", ss.str(), "testListAddToken");
}

void testListTokenIndex()
{
    auto cache = std::make_shared<ll::ScopedVarCache>();
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("index_of (\"are\", [hello there what are you doing]) -> i;", cache, ss);
    ci.parseAndInterpretSingleCommand("string \"you\" -> token;", cache);
    ci.parseAndInterpretSingleCommand("index_of (token, [hello there what are you doing]) -> j;", cache, ss);
    ci.parseAndInterpretSingleCommand("list [hello there what are you doing] -> L;", cache);
    ci.parseAndInterpretSingleCommand("index_of (token, L) -> p;", cache, ss);
    ci.parseAndInterpretSingleCommand("index_of (token, [hello there what are you doing]) -> q;", cache, ss);
    ASSERT_EQUAL(3, *cache->getInt("i"), "index A");
    ASSERT_EQUAL(4, *cache->getInt("j"), "index B");
    ASSERT_EQUAL(4, *cache->getInt("p"), "index C");
    ASSERT_EQUAL(4, *cache->getInt("q"), "index D");
}

int main()
{
    testTypes();
    testMath();
    testVarNewSyntax();
    testVarNewSyntaxFromString();
    testprLiterals();
    testprNLLiterals();
    testprSymbols();
    testprPrimitives(); 
    testprMath();   
    testprString();  
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
    testStartCommand();
    testCallReturnableCommand();
    testCallBlockCommand();
    testCallFunctionNil();
    testArgsCommand();
    testList();
    testListTokenIndex();
    testListGetTokenRaw();
    testListGetTokenSymbol();
    testListSetTokenRaw();
    testListSetTokenSymbol();
    testListAddToken();
    showResults();
    return 0;
}
