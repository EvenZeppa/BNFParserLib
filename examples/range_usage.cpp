#include <iostream>
#include <string>
#include "Grammar.hpp"
#include "BNFParser.hpp"

int main() {
    std::cout << "=== Range-based grammar usage ===" << std::endl;

    Grammar grammar;

    grammar.addRule("<letter> ::= 'a'...'z' | 'A'...'Z'");
    grammar.addRule("<digit> ::= '0'...'9'");
    grammar.addRule("<safechar> ::= ' '...'~'");
    grammar.addRule("<nick-char> ::= <letter> | <digit> | '_' | '-'");
    grammar.addRule("<nickname> ::= <letter> { <nick-char> }");
    grammar.addRule("<trailing> ::= <safechar> { <safechar> }");

    BNFParser parser(grammar);

    size_t consumed = 0;
    ASTNode* nick = parser.parse("<nickname>", "Alice_42", consumed);
    if (nick) {
        std::cout << "Nickname parsed: " << nick->matched << " (" << consumed << " chars)" << std::endl;
        delete nick;
    } else {
        std::cout << "Failed to parse nickname" << std::endl;
    }

    consumed = 0;
    ASTNode* trailing = parser.parse("<trailing>", "Hello, IRC world!", consumed);
    if (trailing) {
        std::cout << "Trailing parsed: " << trailing->matched << " (" << consumed << " chars)" << std::endl;
        delete trailing;
    } else {
        std::cout << "Failed to parse trailing text" << std::endl;
    }

    std::cout << "Range syntax keeps IRC-style character rules concise." << std::endl;
    return 0;
}
