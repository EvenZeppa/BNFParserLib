#include <iostream>
#include <fstream>
#include <vector>
#include "Grammar.hpp"
#include "BNFParserInternal.hpp"
#include "BNFTokenizer.hpp"
#include "Expression.hpp"

// Fonction récursive pour afficher l'AST
void printAST(Expression* expr, int indent = 0) {
    if (!expr) return;

    std::string pad(indent * 2, ' ');
    std::cout << pad << expr->type
              << (expr->value.empty() ? "" : " [match=\"" + expr->value + "\"]")
              << std::endl;

    for (size_t i = 0; i < expr->children.size(); ++i) {
        printAST(expr->children[i], indent + 1);
    }
}

int main() {
    Grammar g;

    std::cout << "=== Construction de la grammaire IRC ===\n";

    // Règles IRC simplifiées pour tests
    g.addRule("<letter> ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H'");
    g.addRule("<number> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'");
    g.addRule("<nick> ::= <letter> { <letter> | <number> }");
    g.addRule("<user> ::= <letter> { <letter> | <number> }");
    g.addRule("<host> ::= <letter> { <letter> | <number> | '.' }");
    g.addRule("<servername> ::= <letter> { <letter> | <number> | '.' }");
    g.addRule("<SPACE> ::= ' ' { ' ' }");
    g.addRule("<middle> ::= <letter> { <letter> | <number> }");
    g.addRule("<trailing> ::= <letter> { <letter> | <number> | ' ' }");
    g.addRule("<prefix> ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]");
    g.addRule("<command> ::= <letter> { <letter> } | <number> <number> <number>");
    g.addRule("<params> ::= <SPACE> [ ':' <trailing> | <middle> <params> ]");
    g.addRule("<crlf> ::= '\\r' '\\n'");
    g.addRule("<message> ::= [ ':' <prefix> <SPACE> ] <command> <params> <crlf>");

    std::cout << "=== Début des tests ===\n";

    // Chargement des tests depuis fichiers
    std::vector<std::string> validTests;
    std::vector<std::string> errorTests;

    std::ifstream validFile("tests/valid_tests.txt");
    std::ifstream errorFile("tests/error_tests.txt");

    std::string line;
    while (std::getline(validFile, line)) validTests.push_back(line);
    while (std::getline(errorFile, line)) errorTests.push_back(line);

    Rule* msgRule = g.getRule("<message>");
    if (!msgRule) {
        std::cerr << "Rule <message> not found!" << std::endl;
        return 1;
    }

    // --- Tests valides ---
    std::cout << "\n--- Valid tests ---\n";
    for (size_t i = 0; i < validTests.size(); ++i) {
        const std::string& input = validTests[i];

        BNFTokenizer tokenizer(input);
        BNFParserInternal parser(tokenizer);
        Expression* ast = parser.parseExpression();

        if (ast) {
            std::cout << input << " : ✔ SUCCESS\n";
            printAST(ast);
            delete ast;
        } else {
            std::cout << input << " : ❌ PARSE ERROR\n";
        }
    }

    // --- Tests erreurs ---
    std::cout << "\n--- Error tests ---\n";
    for (size_t i = 0; i < errorTests.size(); ++i) {
        const std::string& input = errorTests[i];

        BNFTokenizer tokenizer(input);
        BNFParserInternal parser(tokenizer);
        Expression* ast = parser.parseExpression();

        if (ast) {
            std::cout << input << " : ✔ SUCCESS (unexpected)\n";
            printAST(ast);
            delete ast;
        } else {
            std::cout << input << " : ❌ PARSE ERROR (expected)\n";
        }
    }

    std::cout << "\n=== Fin des tests ===\n";
    return 0;
}
