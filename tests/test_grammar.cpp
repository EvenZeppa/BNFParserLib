#include "UnitTest.hpp"
#include "Grammar.hpp"
#include "BNFTokenizer.hpp"
#include "BNFParserInternal.hpp"
#include "Expression.hpp"
#include <string>
#include <vector>
#include <iostream>

// Fonction utilitaire pour compter le nombre de noeuds dans l'AST
int countNodes(Expression* expr) {
    if (!expr) return 0;
    int cnt = 1;
    for (size_t i = 0; i < expr->children.size(); ++i)
        cnt += countNodes(expr->children[i]);
    return cnt;
}

void test_simple_letter_rule() {
    std::cout << "Running test_simple_letter_rule...\n";

    Grammar g;
    g.addRule("<letter> ::= 'A' | 'B' | 'C'");
    Rule* r = g.getRule("<letter>");
    ASSERT_TRUE(r != NULL);
    ASSERT_TRUE(r->rootExpr != NULL);

    Expression* expr = r->rootExpr;
    ASSERT_EQ(expr->type, Expression::EXPR_ALTERNATIVE);
    ASSERT_EQ(expr->children.size(), 3); // A, B, C

    for (size_t i = 0; i < expr->children.size(); ++i)
        ASSERT_EQ(expr->children[i]->type, Expression::EXPR_TERMINAL);
}

void test_simple_nick_rule() {
    std::cout << "Running test_simple_nick_rule...\n";

    Grammar g;
    g.addRule("<letter> ::= 'A' | 'B' | 'C'");
    g.addRule("<number> ::= '0' | '1' | '2'");
    g.addRule("<nick> ::= <letter> { <letter> | <number> }");

    Rule* r = g.getRule("<nick>");
    ASSERT_TRUE(r != NULL);
    ASSERT_TRUE(r->rootExpr != NULL);

    Expression* expr = r->rootExpr;
    ASSERT_EQ(expr->type, Expression::EXPR_REPEAT);
    ASSERT_EQ(expr->children.size(), 1);

    Expression* child = expr->children[0];
    ASSERT_EQ(child->type, Expression::EXPR_ALTERNATIVE);
}

void test_parser_integration() {
    std::cout << "Running test_parser_integration...\n";

    Grammar g;
    g.addRule("<letter> ::= 'A' | 'B'");
    g.addRule("<number> ::= '0' | '1'");
    g.addRule("<nick> ::= <letter> { <letter> | <number> }");

    Rule* r = g.getRule("<nick>");
    ASSERT_TRUE(r != NULL);
    ASSERT_TRUE(r->rootExpr != NULL);

    // Test simple parsing
    std::string input = "AB1";
    BNFTokenizer tokenizer(input);
    BNFParserInternal parser(tokenizer);
    Expression* ast = parser.parseExpression();

    ASSERT_TRUE(ast != NULL);
    ASSERT_EQ(countNodes(ast) > 0, true); // doit avoir au moins un noeud

    delete ast;
}

int main() {
    test_simple_letter_rule();
    test_simple_nick_rule();
    test_parser_integration();

    printTestSummary();
    return (failed == 0) ? 0 : 1;
}
