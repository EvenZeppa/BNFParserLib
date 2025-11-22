#include "Expression.hpp"
#include "UnitTest.hpp"

void test_simple_sequence() {
    Expression* seq = new Expression(Expression::EXPR_SEQUENCE);
    Expression* t1 = new Expression(Expression::EXPR_TERMINAL);
    t1->value = "A";
    Expression* t2 = new Expression(Expression::EXPR_TERMINAL);
    t2->value = "B";
    seq->children.push_back(t1);
    seq->children.push_back(t2);

    ASSERT_EQ(seq->type, Expression::EXPR_SEQUENCE);
    ASSERT_EQ(seq->children.size(), 2u);
    ASSERT_EQ(seq->children[0]->value, std::string("A"));
    ASSERT_EQ(seq->children[1]->value, std::string("B"));

    delete seq;
}

void test_simple_alternative() {
    Expression* alt = new Expression(Expression::EXPR_ALTERNATIVE);
    Expression* t1 = new Expression(Expression::EXPR_TERMINAL);
    t1->value = "X";
    Expression* t2 = new Expression(Expression::EXPR_TERMINAL);
    t2->value = "Y";
    alt->children.push_back(t1);
    alt->children.push_back(t2);

    ASSERT_EQ(alt->type, Expression::EXPR_ALTERNATIVE);
    ASSERT_EQ(alt->children.size(), 2u);
    ASSERT_EQ(alt->children[0]->value, std::string("X"));
    ASSERT_EQ(alt->children[1]->value, std::string("Y"));

    delete alt;
}

void test_nested_expression() {
    Expression* rep = new Expression(Expression::EXPR_REPEAT);
    Expression* term = new Expression(Expression::EXPR_TERMINAL);
    term->value = "Z";
    rep->children.push_back(term);

    ASSERT_EQ(rep->type, Expression::EXPR_REPEAT);
    ASSERT_EQ(rep->children.size(), 1u);
    ASSERT_EQ(rep->children[0]->value, std::string("Z"));

    delete rep;
}

int main() {
    std::cout << "Running test_simple_sequence..." << std::endl;
    test_simple_sequence();

    std::cout << "Running test_simple_alternative..." << std::endl;
    test_simple_alternative();

    std::cout << "Running test_nested_expression..." << std::endl;
    test_nested_expression();

    printTestSummary();
    return 0;
}
