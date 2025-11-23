#include "../include/Grammar.hpp"
#include "../include/BNFParser.hpp"
#include "../include/AST.hpp"
#include "../include/Debug.hpp"
#include <iostream>
#include <string>

/**
 * @brief IRC protocol BNF grammar usage example.
 * 
 * This example demonstrates parsing IRC messages using BNF grammar rules
 * based on RFC 1459 IRC protocol specifications. It shows how to define
 * grammar rules and parse actual IRC message strings.
 */

void setupIRCGrammar(Grammar& grammar) {
    std::cout << "Setting up IRC BNF grammar rules..." << std::endl;
    
    // Basic building blocks
    grammar.addRule("<letter> ::= 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'");
    grammar.addRule("<digit> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'");
    grammar.addRule("<number> ::= <digit> <digit> <digit>");

    // IRC-specific tokens
    grammar.addRule("<crlf> ::= '\\r\\n'");
    grammar.addRule("<space> ::= ' ' { ' ' }");
    
    // Simplified character classes for demonstration
    grammar.addRule("<nick-char> ::= <letter> | <digit> | '_' | '-'");
    grammar.addRule("<nick> ::= <letter> { <nick-char> }");
    grammar.addRule("<user> ::= { <nick-char> }");
    grammar.addRule("<host> ::= { <nick-char> | '.' }");
    grammar.addRule("<servername> ::= { <nick-char> | '.' }");

    // Command can be letters or 3-digit number
    grammar.addRule("<letter-command> ::= <letter> { <letter> }");
    grammar.addRule("<command> ::= <letter-command> | <number>");

    // Prefix: servername or nick[!user][@host]
    grammar.addRule("<nick-prefix> ::= <nick> [ '!' <user> ] [ '@' <host> ]");
    grammar.addRule("<prefix> ::= <servername> | <nick-prefix>");

    // Parameters (simplified for demonstration)
    grammar.addRule("<middle-char> ::= <letter> | <digit> | '_' | '-' | '.' | '#'");
    grammar.addRule("<middle> ::= <middle-char> { <middle-char> }");
    grammar.addRule("<trailing> ::= { <middle-char> | ' ' }");
    grammar.addRule("<params> ::= <space> [ ':' <trailing> | <middle> [ <params> ] ]");
    
    // Main message structure
    grammar.addRule("<message> ::= [ ':' <prefix> <space> ] <command> [ <params> ] <crlf>");

    std::cout << "IRC grammar setup complete!" << std::endl;
}

void parseIRCMessage(const Grammar& grammar, const std::string& ircMessage) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Parsing IRC message: \"" << ircMessage << "\"" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    BNFParser parser(grammar);
    size_t consumed = 0;
    
    ASTNode* ast = parser.parse("<message>", ircMessage, consumed);
    
    if (ast) {
        std::cout << "✓ Parse successful! Consumed " << consumed << " characters." << std::endl;
        std::cout << "\nAST Structure:" << std::endl;
        std::cout << std::string(40, '-') << std::endl;
        printAST(ast);
        delete ast;
    } else {
        std::cout << "✗ Parse failed. Could not match IRC message format." << std::endl;
        std::cout << "  Consumed " << consumed << " characters before failure." << std::endl;
    }
}

void demonstrateSimpleCommands(const Grammar& grammar) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Testing simple IRC commands" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Test individual components first
    std::vector<std::pair<std::string, std::string> > tests;
    tests.push_back(std::make_pair("<nick>", "alice"));
    tests.push_back(std::make_pair("<command>", "PRIVMSG"));
    tests.push_back(std::make_pair("<command>", "001"));
    tests.push_back(std::make_pair("<letter-command>", "JOIN"));
    tests.push_back(std::make_pair("<number>", "332"));
    
    BNFParser parser(grammar);
    
    for (size_t i = 0; i < tests.size(); ++i) {
        const std::pair<std::string, std::string>& test = tests[i];
        std::cout << "\nTesting rule '" << test.first << "' with input '" << test.second << "': ";
        size_t consumed = 0;
        ASTNode* ast = parser.parse(test.first, test.second, consumed);
        
        if (ast) {
            std::cout << "✓ SUCCESS (consumed " << consumed << " chars)" << std::endl;
            delete ast;
        } else {
            std::cout << "✗ FAILED" << std::endl;
        }
    }
}

int main() {
    std::cout << "IRC Protocol BNF Grammar Example" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "\nThis example demonstrates parsing IRC messages using BNF grammar" << std::endl;
    std::cout << "based on RFC 1459 IRC protocol specifications." << std::endl;
    
    try {
        // Create and setup grammar
        Grammar ircGrammar;
        setupIRCGrammar(ircGrammar);
        
        // Test individual components first
        demonstrateSimpleCommands(ircGrammar);
        
        // Test complete IRC messages
        std::vector<std::string> ircMessages;
        // Simple command without prefix
        ircMessages.push_back("PING\\r\\n");
        
        // Command with parameters
        ircMessages.push_back("JOIN #channel\\r\\n");
        
        // Message with prefix
        ircMessages.push_back(":alice!user@host PRIVMSG #channel :Hello World\\r\\n");
        
        // Numeric reply
        ircMessages.push_back("001 alice :Welcome\\r\\n");
        
        // Server message
        ircMessages.push_back(":server.irc.net 332 alice #channel :Channel topic\\r\\n");
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "Testing complete IRC messages" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        for (size_t i = 0; i < ircMessages.size(); ++i) {
            parseIRCMessage(ircGrammar, ircMessages[i]);
        }
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "IRC Grammar Reference" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Based on RFC 1459, the IRC message format is:" << std::endl;
        std::cout << std::endl;
        std::cout << "<message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>" << std::endl;
        std::cout << "<prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]" << std::endl;
        std::cout << "<command>  ::= <letter> { <letter> } | <number> <number> <number>" << std::endl;
        std::cout << "<SPACE>    ::= ' ' { ' ' }" << std::endl;
        std::cout << "<params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]" << std::endl;
        std::cout << "<crlf>     ::= CR LF" << std::endl;
        std::cout << std::endl;
        std::cout << "This example shows a simplified version for demonstration purposes." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nExample completed successfully!" << std::endl;
    return 0;
}
