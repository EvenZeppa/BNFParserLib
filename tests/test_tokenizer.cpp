#include "UnitTest.hpp"
#include "BNFTokenizer.hpp"
#include <string>

void test_single_terminal() {
    BNFTokenizer tz("'A'");
    Token t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_TERMINAL);
    ASSERT_EQ(t.value, "A");
    ASSERT_EQ(tz.next().type, Token::TOK_END);
}

void test_single_symbol() {
    BNFTokenizer tz("<letter>");
    Token t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_SYMBOL);
    ASSERT_EQ(t.value, "<letter>");
    ASSERT_EQ(tz.next().type, Token::TOK_END);
}

void test_word_token() {
    BNFTokenizer tz("WORD");
    Token t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_WORD);
    ASSERT_EQ(t.value, "WORD");
    ASSERT_EQ(tz.next().type, Token::TOK_END);
}

void test_pipe_token() {
    BNFTokenizer tz("|");
    Token t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_PIPE);
    ASSERT_EQ(t.value, "|");
}

void test_braces_and_brackets() {
    BNFTokenizer tz("{ } [ ]");
    Token t1 = tz.next();
    ASSERT_EQ(t1.type, Token::TOK_LBRACE);
    Token t2 = tz.next();
    ASSERT_EQ(t2.type, Token::TOK_RBRACE);
    Token t3 = tz.next();
    ASSERT_EQ(t3.type, Token::TOK_LBRACKET);
    Token t4 = tz.next();
    ASSERT_EQ(t4.type, Token::TOK_RBRACKET);
    ASSERT_EQ(tz.next().type, Token::TOK_END);
}

void test_complex_expression() {
    BNFTokenizer tz("<letter> { <letter> | '0' } [ 'X' ]");
    Token t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_SYMBOL);
    t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_LBRACE);
    t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_SYMBOL);
    t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_PIPE);
    t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_TERMINAL);
    t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_RBRACE);
    t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_LBRACKET);
    t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_TERMINAL);
    t = tz.next();
    ASSERT_EQ(t.type, Token::TOK_RBRACKET);
    ASSERT_EQ(tz.next().type, Token::TOK_END);
}

void test_peek_vs_next() {
    BNFTokenizer tz("'A' | 'B'");
    Token t1 = tz.peek();
    ASSERT_EQ(t1.type, Token::TOK_TERMINAL);
    ASSERT_EQ(t1.value, "A");

    Token t2 = tz.next();
    ASSERT_EQ(t2.type, Token::TOK_TERMINAL);
    ASSERT_EQ(t2.value, "A");

    Token t3 = tz.peek();
    ASSERT_EQ(t3.type, Token::TOK_PIPE);

    Token t4 = tz.next();
    ASSERT_EQ(t4.type, Token::TOK_PIPE);

    Token t5 = tz.next();
    ASSERT_EQ(t5.type, Token::TOK_TERMINAL);
    ASSERT_EQ(t5.value, "B");
}

int main() {
    std::cout << "Running test_single_terminal...\n"; test_single_terminal();
    std::cout << "Running test_single_symbol...\n"; test_single_symbol();
    std::cout << "Running test_word_token...\n"; test_word_token();
    std::cout << "Running test_pipe_token...\n"; test_pipe_token();
    std::cout << "Running test_braces_and_brackets...\n"; test_braces_and_brackets();
    std::cout << "Running test_complex_expression...\n"; test_complex_expression();
    std::cout << "Running test_peek_vs_next...\n"; test_peek_vs_next();

    printTestSummary();
    return failed == 0 ? 0 : 1;
}
