#include "../include/TestFramework.hpp"
#include "../include/Grammar.hpp"
#include "../include/Arena.hpp"
#include <sstream>

// Stress test: build many small rules using an arena to ensure no crashes and consistent parsing.
void test_arena_many_rules(TestRunner& runner) {
    Arena arena(2048);
    Grammar g;
    g.setArena(&arena);

    // Build a bunch of simple rules: <rN> ::= 'A' | 'B'
    const int RULE_COUNT = 500;
    for (int i = 0; i < RULE_COUNT; ++i) {
        std::ostringstream oss;
        oss << "<r" << i << "> ::= 'A' | 'B'";
        g.addRule(oss.str());
    }

    // Spot-check a few rules exist and have the expected shape
    Rule* r0 = g.getRule("<r0>");
    Rule* rLast = g.getRule("<r" + std::string("499") + ">");

    ASSERT_NOT_NULL(runner, r0);
    ASSERT_NOT_NULL(runner, rLast);
    ASSERT_EQ(runner, r0->rootExpr->type, Expression::EXPR_ALTERNATIVE);
    ASSERT_EQ(runner, rLast->rootExpr->type, Expression::EXPR_ALTERNATIVE);
    ASSERT_EQ(runner, r0->rootExpr->children.size(), 2u);
    ASSERT_EQ(runner, rLast->rootExpr->children.size(), 2u);
}

int main() {
    TestSuite suite("Arena Stress Test Suite");
    suite.addTest("Arena Many Rules", test_arena_many_rules);
    TestRunner results = suite.run();
    results.printSummary();
    return results.allPassed() ? 0 : 1;
}
