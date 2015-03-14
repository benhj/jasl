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
    decltype(ll::VarCache::intCache)().swap(ll::VarCache::intCache);
    decltype(ll::VarCache::doubleCache)().swap(ll::VarCache::doubleCache);
    decltype(ll::VarCache::boolCache)().swap(ll::VarCache::boolCache);
    decltype(ll::VarCache::stringCache)().swap(ll::VarCache::stringCache);
    decltype(ll::VarCache::script)().swap(ll::VarCache::script);
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

void testCVarNewSyntax()
{
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("integer(1, i);");
    ci.parseAndInterpretSingleCommand("decimal(1.1, j);");
    ci.parseAndInterpretSingleCommand("boolean(true, k);");
    ASSERT_EQUAL(1, ll::VarCache::intCache["i"], "testCVarNewSyntax::i is 1");
    ASSERT_EQUAL(1.1, ll::VarCache::doubleCache["j"], "testCVarNewSyntax::j is 1.1");
    ASSERT_EQUAL(true, ll::VarCache::boolCache["k"], "testCVarNewSyntax::k is true");
    clearCaches();
}

void testEchoLiterals()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("echo(\"Hello, world!\");", ss);
    ASSERT_EQUAL("Hello, world!", ss.str(), "testEcho: Hello, world!");
    clearCaches();
}

void testEchoNLLiterals()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("echo_nl(\"Hello, world!\");", ss);
    ASSERT_EQUAL("Hello, world!\n", ss.str(), "testEchoNL: Hello, world!");
    clearCaches();
}

void testEchoSymbols()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("var(int, b, 10);");
    ci.parseAndInterpretSingleCommand("var(double, c, 1.1);");
    ci.parseAndInterpretSingleCommand("var(bool, d, true);");
    ci.parseAndInterpretSingleCommand("echo(b);", ss);
    ci.parseAndInterpretSingleCommand("echo(c);", ss);
    ci.parseAndInterpretSingleCommand("echo_nl(d);", ss);
    ASSERT_EQUAL("101.11\n", ss.str(), "testEchoSymbols");
    clearCaches();
}

void testEchoPrimitives()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("echo_nl(2);", ss);
    ci.parseAndInterpretSingleCommand("echo_nl(5.67);", ss);
    ci.parseAndInterpretSingleCommand("echo_nl(3.14);", ss);
    ASSERT_EQUAL("2\n5.67\n3.14\n", ss.str(), "testEchoPrimitives");
    clearCaches();
}

void testEchoMath()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("echo_nl((5 + 2) * 5.6);", ss);
    ASSERT_EQUAL("39.2\n", ss.str(), "testEchoMath");
    clearCaches();
}

void testEchoString()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string(\"Hello, world!\", hello);", ss);
    ci.parseAndInterpretSingleCommand("echo_nl(hello);", ss);
    ASSERT_EQUAL("Hello, world!\n", ss.str(), "testEchoString");
    clearCaches();
}

void testStringWithLiteral()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string(\"Hello, world!\", hello);", ss);
    ASSERT_EQUAL("Hello, world!", ll::VarCache::stringCache["hello"], "testStringWithLiteral");
    clearCaches();
}

void testStringWithNumbers()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string(5, numberFive);", ss);
    ASSERT_EQUAL("5", ll::VarCache::stringCache["numberFive"], "testStringWithNumbers integer");
    ci.parseAndInterpretSingleCommand("string(5.5, numberDecimal);", ss);
    ASSERT_EQUAL("5.5", ll::VarCache::stringCache["numberDecimal"], "testStringWithNumbers decimal");
    clearCaches();
}

void testStringWithMath()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string((5 + 2) * 5.6, numberMath);", ss);
    ci.parseAndInterpretSingleCommand("string((5 + 2) * 5, numberMathB);", ss);
    ASSERT_EQUAL("39.2", ll::VarCache::stringCache["numberMath"], "testStringWithMath A");
    ASSERT_EQUAL("35", ll::VarCache::stringCache["numberMathB"], "testStringWithMath B");
    clearCaches();
}

void testLiteralStringToInteger()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string_to_integer(\"123\", a);", ss);
    ASSERT_EQUAL(123, ll::VarCache::intCache["a"], "testLiteralStringToInteger");
    clearCaches();
}

void testLiteralStringToDecimal()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string_to_decimal(\"3.14\", a);", ss);
    ASSERT_EQUAL(3.14, ll::VarCache::doubleCache["a"], "testLiteralStringToDecimal");
    clearCaches();
}

void testSymbolStringToInteger()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string(\"456\", a);", ss);
    ci.parseAndInterpretSingleCommand("string_to_integer(a, s);", ss);
    ASSERT_EQUAL(456, ll::VarCache::intCache["s"], "testSymbolStringToInteger");
    clearCaches();
}


void testSymbolStringToDecimal()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string(\"3.14\", a);", ss);
    ci.parseAndInterpretSingleCommand("string_to_decimal(a, s);", ss);
    ASSERT_EQUAL(3.14, ll::VarCache::doubleCache["s"], "testSymbolStringToDecimal");
    clearCaches();
}

void testAppendLiteral()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string(\"Hello\", s);", ss);
    ci.parseAndInterpretSingleCommand("string_append(s, \", world!\");", ss);
    ASSERT_EQUAL("Hello, world!", ll::VarCache::stringCache["s"], "testAppendLiteral");
    clearCaches();
}

void testAppendString()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string(\"Hello\", s);", ss);
    ci.parseAndInterpretSingleCommand("string(\", world!\", b);", ss);
    ci.parseAndInterpretSingleCommand("string_append(s, b);", ss);
    ASSERT_EQUAL("Hello, world!", ll::VarCache::stringCache["s"], "testAppendString");
    clearCaches();
}

void testReverseString()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string(\"Hello\", s);", ss);
    ci.parseAndInterpretSingleCommand("string_reverse(s);", ss);
    ASSERT_EQUAL("olleH", ll::VarCache::stringCache["s"], "testReverseString");
    clearCaches();
}

void testStringLength()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("string_length(\"Hello\", s);", ss);
    ci.parseAndInterpretSingleCommand("string(\"Hello, world!\", sb);", ss);
    ci.parseAndInterpretSingleCommand("string_length(sb, sc);", ss);
    ASSERT_EQUAL(5, ll::VarCache::intCache["s"], "testStringLength A");
    ASSERT_EQUAL(13, ll::VarCache::intCache["sc"], "testStringLength B");
    clearCaches();
}

void testIfCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ci.parseAndInterpretSingleCommand("if(1 < 2) { echo(\"Hello, world!\"); var(int, x, 5); }", ss);
    ASSERT_EQUAL("Hello, world!", ss.str(), "testIfCommand: Hello, world!");
    ASSERT_EQUAL(5, ll::VarCache::intCache["x"], "testIfCommand: x is 5");
    clearCaches();
}

void testRepeatCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string const commands("var(int, x, 0); repeat 5 times { echo(\"Hello\"); var(int, x, (x + 1)); }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        (void)ci.interpretFunc(f, ss);
    }
    ASSERT_EQUAL("HelloHelloHelloHelloHello", ss.str(), "testRepeatCommand::repeat hello");
    ASSERT_EQUAL(5, ll::VarCache::intCache["x"], "testRepeatCommand::x is 5");
    clearCaches();
}

void testWhileCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string const commands("var(int, x, 5); while(x > 0) { var(int, x, (x - 1)); }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        (void)ci.interpretFunc(f, ss);
    }
    ASSERT_EQUAL(0, ll::VarCache::intCache["x"], "testWhileCommand::x is 0");
    clearCaches();
}

void testBlockCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string commands("var(int, x, 0); block foo { echo(\"Hello\"); var(int, x, (x + 1)); }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        (void)ci.interpretFunc(f, ss);
    }
    ASSERT_EQUAL("Hello", ss.str(), "testBlockCommand::print hello");
    ASSERT_EQUAL(1, ll::VarCache::intCache["x"], "testBlockCommand::x is 1");
    clearCaches();
}

void testStartCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string const command("start { echo(\"Hello\"); var(int, x, 21); }");
    ci.parseAndInterpretSingleCommand(command, ss);
    ASSERT_EQUAL("Hello", ss.str(), "testStartCommand::print hello");
    ASSERT_EQUAL(21, ll::VarCache::intCache["x"], "testStartCommand::x is 21");
    clearCaches();
}

void testCallCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    std::string commands("start { echo(\"Starting..\"); call foo; call bar; var(int, x, (x - 5));}");
    commands.append("block bar { echo(\"..and Goodbye!\"); var(int, x, (x + 1)); }");
    commands.append("block foo { echo(\"Hello\"); var(int, x, 20); }");
    auto functions = ci.parseStringCollection(commands);
    for(auto &f : functions) {
        if(f.name == "start") {
            (void)ci.interpretFunc(f, ss);
            break;
        }
    }
    ASSERT_EQUAL("Starting..Hello..and Goodbye!", ss.str(), "testCallCommand::print-outs");
    ASSERT_EQUAL(16, ll::VarCache::intCache["x"], "testStartCommand::x is 16");
    clearCaches();
}

void testArgsCommand()
{
    std::ostringstream ss;
    ll::CommandInterpretor ci;
    ll::VarCache::args.push_back("argument1");
    ll::VarCache::args.push_back("argument2");
    ci.parseAndInterpretSingleCommand("args(0, arga);", ss);
    ci.parseAndInterpretSingleCommand("args(1, argb);", ss);
    ASSERT_EQUAL("argument1", ll::VarCache::stringCache["arga"], "testArgsCommand A");
    ASSERT_EQUAL("argument2", ll::VarCache::stringCache["argb"], "testArgsCommand B");
    clearCaches();
}

int main()
{
    testVarCommand();
    testMathCommands();
    testCVarCommandsBasic();
    testCVarCommandsCompound();
    testCVarNewSyntax();
    testEchoLiterals();
    testEchoNLLiterals();
    testEchoSymbols();
    testEchoPrimitives(); 
    testEchoMath();   
    testEchoString();  
    testStringWithLiteral();
    testStringWithNumbers();
    testStringWithMath();
    testLiteralStringToInteger();
    testSymbolStringToInteger();
    testLiteralStringToDecimal();
    testSymbolStringToDecimal();
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
    showResults();
    return 0;
}