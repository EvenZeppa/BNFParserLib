#include "Grammar.hpp"
#include "BNFParser.hpp"
#include "DataExtractor.hpp"
#include "ExtractedData.hpp"
#include "Debug.hpp"
#include <iostream>

/**
 * @brief Main demonstration program for BNF interpreter with data extraction.
 * 
 * Shows how to define grammar rules, parse input text, and extract structured
 * data from the resulting AST. Uses C++98 compatible code throughout.
 */

void printExtractedData(const ExtractedData& data) {
    std::cout << "\nExtracted symbols:\n";
    std::cout << std::string(40, '-') << std::endl;
    
    // C++98 compatible iteration over map
    for (std::map<std::string, std::vector<std::string> >::const_iterator it = data.values.begin();
         it != data.values.end(); ++it) {
        
        const std::string& sym = it->first;
        const std::vector<std::string>& vec = it->second;
        
        std::cout << sym << " => ";
        
        // C++98 compatible iteration over vector
        for (size_t i = 0; i < vec.size(); ++i) {
            std::cout << "\"" << vec[i] << "\"";
            if (i < vec.size() - 1) std::cout << ", ";
        }
        std::cout << " (" << vec.size() << " occurrence(s))" << std::endl;
    }
}

void setupComplexGrammar(Grammar& g) {
    std::cout << "Setting up complex IRC-like grammar with repetitions..." << std::endl;
    
    // Basic building blocks
    g.addRule("<letter> ::= 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'");
    g.addRule("<digit> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'");
    g.addRule("<special> ::= '#' | '@' | '!' | '.' | '-' | '_'");
    
    // Character classes for different components
    g.addRule("<nick-char> ::= <letter> | <digit> | '_' | '-'");
    g.addRule("<command-char> ::= <letter> | <digit>");
    g.addRule("<param-char> ::= <letter> | <digit> | <special>");
    
    // Complex structures with repetitions
    g.addRule("<nick> ::= <letter> { <nick-char> }");
    g.addRule("<command> ::= <command-char> { <command-char> }");
    g.addRule("<param> ::= <param-char> { <param-char> }");
    g.addRule("<channel> ::= '#' <letter> { <nick-char> }");
    
    // Spaces and separators
    g.addRule("<space> ::= ' '");
    g.addRule("<spaces> ::= <space> { <space> }");
    g.addRule("<crlf> ::= '\\r\\n'");
    
    // User identification
    g.addRule("<user> ::= <nick-char> { <nick-char> }");
    g.addRule("<host> ::= <nick-char> { <nick-char> | '.' }");
    g.addRule("<prefix> ::= <nick> [ '!' <user> ] [ '@' <host> ]");
    
    // Parameter lists with repetitions
    g.addRule("<param-list> ::= <param> { <spaces> <param> }");
    g.addRule("<trailing> ::= ':' { <param-char> | <space> }");
    g.addRule("<params> ::= [ <spaces> <param-list> ] [ <spaces> <trailing> ]");
    
    // Complete IRC message with optional prefix and parameters
    g.addRule("<message> ::= [ ':' <prefix> <spaces> ] <command> <params> <crlf>");
}

void demonstrateExtractorConfigurations(Grammar& g, const std::string& complexMessage) {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "DEMONSTRATING DATAEXTRACTOR CONFIGURATIONS" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "Using message: \"" << complexMessage << "\"" << std::endl;

    BNFParser parser(g);
    size_t consumed = 0;
    ASTNode* ast = parser.parse("<message>", complexMessage, consumed);

    if (!ast) {
        std::cout << "Parse failed for configuration demo" << std::endl;
        return;
    }

    std::cout << "\nAST Structure:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    // printAST(ast);

    // Configuration 1: Default extraction (non-terminals only)
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "1. DEFAULT CONFIGURATION (non-terminals only)" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    DataExtractor extractor1;
    ExtractedData data1 = extractor1.extract(ast);
    printExtractedData(data1);

    // Configuration 2: Include terminals
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "2. INCLUDING TERMINALS" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    DataExtractor extractor2;
    extractor2.includeTerminals(true);
    ExtractedData data2 = extractor2.extract(ast);
    printExtractedData(data2);

    // Configuration 3: Specific symbols only
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "3. SPECIFIC SYMBOLS ONLY" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    DataExtractor extractor3;
    std::vector<std::string> targetSymbols;
    targetSymbols.push_back("<command>");
    targetSymbols.push_back("<nick>");
    targetSymbols.push_back("<channel>");
    targetSymbols.push_back("<param>");
    extractor3.setSymbols(targetSymbols);
    ExtractedData data3 = extractor3.extract(ast);
    printExtractedData(data3);
    
    // Show utility methods for specific symbols
    if (data3.has("<command>")) {
        std::cout << "Command utility: first='" << data3.first("<command>") << "', count=" << data3.count("<command>") << std::endl;
    }
    if (data3.has("<param>")) {
        std::cout << "Param utility: first='" << data3.first("<param>") << "', count=" << data3.count("<param>") << std::endl;
        std::vector<std::string> allParams = data3.all("<param>");
        std::cout << "All params: ";
        for (size_t i = 0; i < allParams.size(); ++i) {
            std::cout << "'" << allParams[i] << "'";
            if (i < allParams.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }

    // Configuration 4: Flatten repetitions
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "4. FLATTEN REPETITIONS" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    DataExtractor extractor4;
    extractor4.flattenRepetitions(true);
    extractor4.includeTerminals(true);
    ExtractedData data4 = extractor4.extract(ast);
    printExtractedData(data4);

    // Configuration 5: Combined settings
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "5. COMBINED CONFIGURATION" << std::endl;
    std::cout << "   (specific symbols + terminals + flatten)" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    DataExtractor extractor5;
    extractor5.setSymbols(targetSymbols);
    extractor5.includeTerminals(true);
    extractor5.flattenRepetitions(true);
    ExtractedData data5 = extractor5.extract(ast);
    printExtractedData(data5);

    // Configuration 6: Reset and reconfigure
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "6. RESET AND RECONFIGURE" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    DataExtractor extractor6;
    extractor6.includeTerminals(true);
    extractor6.flattenRepetitions(true);
    std::cout << "Before reset:" << std::endl;
    ExtractedData dataBefore = extractor6.extract(ast);
    std::cout << "Terminal count: " << dataBefore.values.size() << " symbol types" << std::endl;
    
    extractor6.resetConfig();
    std::cout << "After reset:" << std::endl;
    ExtractedData dataAfter = extractor6.extract(ast);
    std::cout << "Symbol count: " << dataAfter.values.size() << " symbol types (should be fewer)" << std::endl;
    printExtractedData(dataAfter);

    delete ast;
}

int main() {
    std::cout << "BNF Interpreter Data Extraction Example (C++98)" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << "\nThis example demonstrates:" << std::endl;
    std::cout << "- Parsing with BNF grammar" << std::endl;
    std::cout << "- Extracting structured data from AST" << std::endl;
    std::cout << "- C++98 compatible implementation" << std::endl;

    try {
        Grammar g;
        setupComplexGrammar(g);

        BNFParser parser(g);

        // Complex test messages with repetitions and multiple parameters
        std::vector<std::string> testMessages;
        testMessages.push_back(":alice!user@host.com PRIVMSG #channel param1 param2 :Hello World\\r\\n");
        testMessages.push_back("JOIN #test-channel\\r\\n");
        testMessages.push_back(":server.irc.net MODE #channel +o alice\\r\\n");
        testMessages.push_back("QUIT :Goodbye everyone\\r\\n");

        for (size_t i = 0; i < testMessages.size(); ++i) {
            std::string msg = testMessages[i];
            std::cout << "\n" << std::string(60, '=') << std::endl;
            std::cout << "TESTING MESSAGE " << (i + 1) << std::endl;
            std::cout << "Message: \"" << msg << "\"" << std::endl;
            std::cout << std::string(60, '=') << std::endl;

            size_t consumed = 0;
            ASTNode* ast = parser.parse("<message>", msg, consumed);

            if (!ast) {
                std::cout << "Parse FAILED" << std::endl;
                std::cout << "Consumed " << consumed << " characters before failure" << std::endl;
                std::cout << "Note: This complex grammar may not match all messages perfectly." << std::endl;
                continue;
            }

            std::cout << "Parse succeeded! Consumed " << consumed << " characters" << std::endl;

            // Print AST structure (limited depth for readability)
            std::cout << "\nAST Structure:" << std::endl;
            std::cout << std::string(30, '-') << std::endl;
            // printAST(ast);

            // Basic extraction
            std::cout << "\nBASIC EXTRACTION:" << std::endl;
            DataExtractor extractor;
            ExtractedData data = extractor.extract(ast);
            printExtractedData(data);

            // Demonstrate utility methods for common symbols
            std::vector<std::string> commonSymbols;
            commonSymbols.push_back("<command>");
            commonSymbols.push_back("<nick>");
            commonSymbols.push_back("<channel>");
            commonSymbols.push_back("<param>");
            commonSymbols.push_back("<user>");
            commonSymbols.push_back("<host>");
            
            std::cout << "\nSYMBOL ANALYSIS:" << std::endl;
            for (size_t j = 0; j < commonSymbols.size(); ++j) {
                const std::string& sym = commonSymbols[j];
                if (data.has(sym)) {
                    std::cout << "  " << sym << ": \"" << data.first(sym) << "\" (count: " << data.count(sym) << ")" << std::endl;
                }
            }

            delete ast;
        }

        // Run comprehensive configuration demonstration with the most complex message
        std::string complexMessage = ":alice!user@host.com PRIVMSG #channel param1 param2 :Hello World\\r\\n";
        demonstrateExtractorConfigurations(g, complexMessage);

        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "EXAMPLE COMPLETED SUCCESSFULLY!" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        std::cout << "This example demonstrated:" << std::endl;
        std::cout << "✓ Complex BNF grammar with repetitions and alternatives" << std::endl;
        std::cout << "✓ Parsing IRC-like messages with multiple components" << std::endl;
        std::cout << "✓ DataExtractor configuration options:" << std::endl;
        std::cout << "  - setSymbols(): Filter specific symbols" << std::endl;
        std::cout << "  - includeTerminals(): Include/exclude terminal symbols" << std::endl;
        std::cout << "  - flattenRepetitions(): Flatten repetition structures" << std::endl;
        std::cout << "  - resetConfig(): Reset to default settings" << std::endl;
        std::cout << "✓ ExtractedData utility methods:" << std::endl;
        std::cout << "  - has(): Check symbol existence" << std::endl;
        std::cout << "  - first(): Get first occurrence" << std::endl;
        std::cout << "  - all(): Get all occurrences" << std::endl;
        std::cout << "  - count(): Get occurrence count" << std::endl;
        std::cout << "✓ Full C++98 compatibility with std::map and explicit types" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
