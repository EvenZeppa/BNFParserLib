/**
 * Example: Partial Parsing with ParseContext
 * 
 * Demonstrates how to use the partial parsing feature to:
 *   1. Extract valid elements from partially valid inputs
 *   2. Get detailed information about what failed and where
 *   3. Handle mixed valid/invalid data gracefully
 * 
 * Use cases:
 *   - IRC channel lists where some channels are invalid
 *   - CSV data with some malformed rows
 *   - Configuration files with some invalid entries
 *   - Any input where you want to process what's valid and report what's invalid
 */

#include <iostream>
#include <string>
#include "Grammar.hpp"
#include "BNFParser.hpp"
#include "ParseContext.hpp"
#include "AST.hpp"

void showPartialNodes(const ParseContext& ctx) {
    if (ctx.partialNodes.empty()) {
        std::cout << "  No partial nodes (either full success or early failure)" << std::endl;
        return;
    }
    
    std::cout << "  Partial nodes (" << ctx.partialNodes.size() << " successfully parsed):" << std::endl;
    for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
        std::cout << "    [" << i << "] " << ctx.partialNodes[i]->symbol 
                  << " => '" << ctx.partialNodes[i]->matched << "'" << std::endl;
    }
}

void showFailures(const ParseContext& ctx) {
    if (ctx.failures.empty()) {
        std::cout << "  No failures detected" << std::endl;
        return;
    }
    
    std::cout << "  Failures (" << ctx.failures.size() << " detected):" << std::endl;
    for (size_t i = 0; i < ctx.failures.size(); ++i) {
        const FailedNode& fail = ctx.failures[i];
        std::cout << "    [" << i << "] Position " << fail.position 
                  << ": '" << fail.text << "'" << std::endl;
        std::cout << "        Expected: " << fail.expected << std::endl;
        std::cout << "        While parsing: " << fail.ruleName << std::endl;
    }
}

int main() {
    std::cout << "=== Partial Parsing Examples ===" << std::endl;
    std::cout << std::endl;

    // Example 1: IRC channel list with invalid channel names
    {
        std::cout << "Example 1: IRC JOIN command with mixed valid/invalid channels" << std::endl;
        std::cout << "---------------------------------------------------------------" << std::endl;
        
        Grammar g;
        g.addRule("<letter> ::= 'a' ... 'z' | 'A' ... 'Z'");
        g.addRule("<digit> ::= '0' ... '9'");
        g.addRule("<channelchar> ::= <letter> | <digit> | '_' | '-'");
        g.addRule("<channel> ::= '#' <letter> { <channelchar> }");
        g.addRule("<comma> ::= ','");
        g.addRule("<chanlist> ::= <channel> { <comma> <channel> }");
        
        BNFParser parser(g);
        ParseContext ctx;
        
        // Input: #chan1,invalid,#chan3
        // "invalid" doesn't start with '#', should fail
        std::string input = "#chan1,invalid,#chan3";
        parser.parse("<chanlist>", input, ctx);
        
        std::cout << "Input: '" << input << "'" << std::endl;
        std::cout << "Success: " << (ctx.success ? "yes" : "no") << std::endl;
        std::cout << "Consumed: " << ctx.consumed << " characters" << std::endl;
        std::cout << std::endl;
        
        showPartialNodes(ctx);
        std::cout << std::endl;
        showFailures(ctx);
        std::cout << std::endl;
        
        std::cout << "Analysis:" << std::endl;
        std::cout << "  - Parsed '#chan1' successfully" << std::endl;
        std::cout << "  - Failed on 'invalid' (doesn't start with '#')" << std::endl;
        std::cout << "  - Couldn't reach '#chan3' due to earlier failure" << std::endl;
        std::cout << "  - partialNodes contains successfully parsed channels before failure" << std::endl;
        std::cout << std::endl;
        
        if (ctx.ast) delete ctx.ast;
        for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
            delete ctx.partialNodes[i];
        }
    }

    // Example 2: List of numbers with invalid entry
    {
        std::cout << "Example 2: Number list with invalid entry" << std::endl;
        std::cout << "------------------------------------------" << std::endl;
        
        Grammar g;
        g.addRule("<digit> ::= '0' ... '9'");
        g.addRule("<number> ::= <digit> { <digit> }");
        g.addRule("<comma> ::= ','");
        g.addRule("<numlist> ::= <number> { <comma> <number> }");
        
        BNFParser parser(g);
        ParseContext ctx;
        
        std::string input = "123,456,abc,789";
        parser.parse("<numlist>", input, ctx);
        
        std::cout << "Input: '" << input << "'" << std::endl;
        std::cout << "Success: " << (ctx.success ? "yes" : "no") << std::endl;
        std::cout << "Consumed: " << ctx.consumed << " characters (matched '123,456')" << std::endl;
        std::cout << std::endl;
        
        showPartialNodes(ctx);
        std::cout << std::endl;
        showFailures(ctx);
        std::cout << std::endl;
        
        std::cout << "Analysis:" << std::endl;
        std::cout << "  - Successfully parsed '123' and '456'" << std::endl;
        std::cout << "  - Failed at 'abc' (not a valid number)" << std::endl;
        std::cout << "  - '789' couldn't be reached" << std::endl;
        std::cout << std::endl;
        
        if (ctx.ast) delete ctx.ast;
        for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
            delete ctx.partialNodes[i];
        }
    }

    // Example 3: Sequence failure after partial success
    {
        std::cout << "Example 3: Structured data with incomplete record" << std::endl;
        std::cout << "---------------------------------------------------" << std::endl;
        
        Grammar g;
        g.addRule("<letter> ::= 'a' ... 'z' | 'A' ... 'Z'");
        g.addRule("<digit> ::= '0' ... '9'");
        g.addRule("<name> ::= <letter> { <letter> }");
        g.addRule("<age> ::= <digit> { <digit> }");
        g.addRule("<space> ::= ' '");
        g.addRule("<record> ::= <name> <space> <age>");
        
        BNFParser parser(g);
        ParseContext ctx;
        
        std::string input = "John "; // Missing age
        parser.parse("<record>", input, ctx);
        
        std::cout << "Input: '" << input << "' (expecting '<name> <space> <age>')" << std::endl;
        std::cout << "Success: " << (ctx.success ? "yes" : "no") << std::endl;
        std::cout << "Consumed: " << ctx.consumed << " characters" << std::endl;
        std::cout << std::endl;
        
        showPartialNodes(ctx);
        std::cout << std::endl;
        showFailures(ctx);
        std::cout << std::endl;
        
        std::cout << "Analysis:" << std::endl;
        std::cout << "  - Parsed 'John' (name) successfully" << std::endl;
        std::cout << "  - Parsed ' ' (space) successfully" << std::endl;
        std::cout << "  - Failed to find required age (end of input)" << std::endl;
        std::cout << "  - partialNodes contains the valid parsed elements" << std::endl;
        std::cout << std::endl;
        
        if (ctx.ast) delete ctx.ast;
        for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
            delete ctx.partialNodes[i];
        }
    }

    // Example 4: Successful parsing (no partial nodes)
    {
        std::cout << "Example 4: Fully successful parse" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        
        Grammar g;
        g.addRule("<digit> ::= '0' ... '9'");
        g.addRule("<number> ::= <digit> { <digit> }");
        g.addRule("<comma> ::= ','");
        g.addRule("<numlist> ::= <number> { <comma> <number> }");
        
        BNFParser parser(g);
        ParseContext ctx;
        
        std::string input = "123,456,789";
        parser.parse("<numlist>", input, ctx);
        
        std::cout << "Input: '" << input << "'" << std::endl;
        std::cout << "Success: " << (ctx.success ? "yes" : "no") << std::endl;
        std::cout << "Consumed: " << ctx.consumed << " characters" << std::endl;
        std::cout << std::endl;
        
        showPartialNodes(ctx);
        std::cout << std::endl;
        showFailures(ctx);
        std::cout << std::endl;
        
        std::cout << "Analysis:" << std::endl;
        std::cout << "  - Full parse succeeded" << std::endl;
        std::cout << "  - partialNodes is empty (only populated on partial failures)" << std::endl;
        std::cout << "  - Use ctx.ast to access the full AST" << std::endl;
        std::cout << std::endl;
        
        if (ctx.ast) {
            std::cout << "Full AST:" << std::endl;
            printAST(ctx.ast);
            delete ctx.ast;
        }
        
        for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
            delete ctx.partialNodes[i];
        }
    }

    std::cout << "============================================" << std::endl;
    std::cout << "Partial Parsing Benefits:" << std::endl;
    std::cout << "  • Extract valid data from partially invalid inputs" << std::endl;
    std::cout << "  • Detailed error reporting with positions" << std::endl;
    std::cout << "  • Process what's valid, report what's invalid" << std::endl;
    std::cout << "  • Perfect for user input validation" << std::endl;
    std::cout << "  • Backward compatible - use ctx.ast for normal parsing" << std::endl;

    return 0;
}
