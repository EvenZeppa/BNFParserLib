#include "../include/TestFramework.hpp"
#include "../include/Grammar.hpp"
#include "../include/BNFParser.hpp"
#include <string>

void test_basic_alphabetic_range(TestRunner& runner) {
    Grammar g;
    g.addRule("<letter> ::= 'a'...'z'");

    BNFParser p(g);
    size_t consumed = 0;

    ASTNode* ast = p.parse("<letter>", "a", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, consumed, 1);
    delete ast;

    consumed = 0;
    ast = p.parse("<letter>", "m", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "m");
    delete ast;

    consumed = 0;
    ast = p.parse("<letter>", "z", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "z");
    delete ast;

    consumed = 0;
    ast = p.parse("<letter>", "A", consumed);
    ASSERT_TRUE(runner, ast == 0);
}

void test_numeric_range(TestRunner& runner) {
    Grammar g;
    g.addRule("<digit> ::= '0'...'9'");

    BNFParser p(g);
    size_t consumed = 0;

    ASTNode* ast = p.parse("<digit>", "0", consumed);
    ASSERT_TRUE(runner, ast != 0);
    delete ast;

    consumed = 0;
    ast = p.parse("<digit>", "5", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "5");
    delete ast;

    consumed = 0;
    ast = p.parse("<digit>", "9", consumed);
    ASSERT_TRUE(runner, ast != 0);
    delete ast;

    consumed = 0;
    ast = p.parse("<digit>", "x", consumed);
    ASSERT_TRUE(runner, ast == 0);
}

void test_extended_ascii_symbol_range(TestRunner& runner) {
    Grammar g;
    g.addRule("<printable> ::= ' '...'~'");

    BNFParser p(g);
    size_t consumed = 0;

    ASTNode* ast = p.parse("<printable>", " ", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, consumed, 1);
    delete ast;

    consumed = 0;
    ast = p.parse("<printable>", "A", consumed);
    ASSERT_TRUE(runner, ast != 0);
    delete ast;

    consumed = 0;
    ast = p.parse("<printable>", "~", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "~");
    delete ast;

    consumed = 0;
    std::string newline;
    newline.push_back('\n');
    ast = p.parse("<printable>", newline, consumed);
    ASSERT_TRUE(runner, ast == 0);
}

void test_mixed_alphanum_rule(TestRunner& runner) {
    Grammar g;
    g.addRule("<alphanum> ::= 'a'...'z' | '0'...'9'");

    BNFParser p(g);
    size_t consumed = 0;

    ASTNode* ast = p.parse("<alphanum>", "b", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "b");
    delete ast;

    consumed = 0;
    ast = p.parse("<alphanum>", "7", consumed);
    ASSERT_TRUE(runner, ast != 0);
    ASSERT_EQ(runner, ast->matched, "7");
    delete ast;

    consumed = 0;
    ast = p.parse("<alphanum>", "_", consumed);
    ASSERT_TRUE(runner, ast == 0);
}

int main() {
    TestSuite suite("Range Expression Test Suite");

    suite.addTest("Basic alphabetic range: 'a'...'z'", test_basic_alphabetic_range);
    suite.addTest("Numeric range: '0'...'9'", test_numeric_range);
    suite.addTest("Extended ASCII range: ' '...'~'", test_extended_ascii_symbol_range);
    suite.addTest("Mixed alternatives with ranges", test_mixed_alphanum_rule);

    TestRunner results = suite.run();
    results.printSummary();

    return results.allPassed() ? 0 : 1;
}
