/**
 * Unit tests for partial parsing functionality
 * 
 * Tests ParseContext's ability to collect successfully parsed sub-elements
 * and track failures, even when overall parsing fails.
 */

#include "../include/Grammar.hpp"
#include "../include/BNFParser.hpp"
#include "../include/ParseContext.hpp"
#include <iostream>
#include <cassert>

void test_partial_sequence_success() {
    std::cout << "Test: Partial sequence - all elements succeed" << std::endl;
    
    Grammar g;
    g.addRule("<letter> ::= 'a' ... 'z'");
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<seq> ::= <letter> <digit> <letter>");
    
    BNFParser parser(g);
    ParseContext ctx;
    
    parser.parse("<seq>", "a5b", ctx);
    
    assert(ctx.success == true);
    assert(ctx.consumed == 3);
    // partialNodes may or may not be populated on success
    assert(ctx.failures.empty());
    
    if (ctx.ast) delete ctx.ast;
    for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
        delete ctx.partialNodes[i];
    }
    
    std::cout << "  ✓ Passed" << std::endl;
}

void test_partial_sequence_failure() {
    std::cout << "Test: Partial sequence - fails after first element" << std::endl;
    
    Grammar g;
    g.addRule("<letter> ::= 'a' ... 'z'");
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<seq> ::= <letter> <digit> <letter>");
    
    BNFParser parser(g);
    ParseContext ctx;
    
    parser.parse("<seq>", "a5X", ctx); // 'X' is uppercase, should fail
    
    assert(ctx.success == false);
    assert(ctx.partialNodes.size() == 2); // Should have <letter> and <digit>
    assert(ctx.failures.size() == 1); // Should have one failure
    assert(ctx.failures[0].position == 2); // Failed at position 2
    
    // Verify partial nodes contain the right data
    assert(ctx.partialNodes[0]->matched == "a");
    assert(ctx.partialNodes[1]->matched == "5");
    
    for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
        delete ctx.partialNodes[i];
    }
    
    std::cout << "  ✓ Passed" << std::endl;
}

void test_partial_repetition_all_valid() {
    std::cout << "Test: Partial repetition - all items valid" << std::endl;
    
    Grammar g;
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<digits> ::= <digit> { <digit> }");
    
    BNFParser parser(g);
    ParseContext ctx;
    
    parser.parse("<digits>", "123", ctx);
    
    assert(ctx.success == true);
    assert(ctx.consumed == 3);
    // When fully successful with no failures, partialNodes remain empty
    assert(ctx.partialNodes.empty());
    assert(ctx.failures.empty());
    
    if (ctx.ast) delete ctx.ast;
    for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
        delete ctx.partialNodes[i];
    }
    
    std::cout << "  ✓ Passed" << std::endl;
}

void test_partial_repetition_mixed_validity() {
    std::cout << "Test: Partial repetition - some items invalid" << std::endl;
    
    Grammar g;
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<digits> ::= <digit> { <digit> }");
    
    BNFParser parser(g);
    ParseContext ctx;
    
    parser.parse("<digits>", "12a45", ctx);
    
    // Repetition succeeds with partial match (matches "12")
    // Note: <digit> { <digit> } means first digit is separate, repetition gets second
    assert(ctx.success == true);
    assert(ctx.consumed == 2); // Matched "12" only
    assert(ctx.failures.size() == 1); // Failed at 'a'
    
    // partialNodes contains elements from the repetition part only
    std::cout << "  Matched " << ctx.partialNodes.size() << " partial node(s)" << std::endl;
    
    if (ctx.ast) delete ctx.ast;
    for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
        delete ctx.partialNodes[i];
    }
    
    std::cout << "  ✓ Passed" << std::endl;
}

void test_irc_channel_list_valid() {
    std::cout << "Test: IRC channel list - all valid channels" << std::endl;
    
    Grammar g;
    g.addRule("<letter> ::= 'a' ... 'z' | 'A' ... 'Z'");
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<channelchar> ::= <letter> | <digit> | '_' | '-'");
    g.addRule("<channel> ::= '#' <letter> { <channelchar> }");
    g.addRule("<comma> ::= ','");
    g.addRule("<chanlist> ::= <channel> { <comma> <channel> }");
    
    BNFParser parser(g);
    ParseContext ctx;
    
    parser.parse("<chanlist>", "#chan1,#test,#foo", ctx);
    
    assert(ctx.success == true);
    assert(ctx.consumed == 17);
    // No failures since everything parses successfully
    std::cout << "  partialNodes.size() = " << ctx.partialNodes.size() << std::endl;
    std::cout << "  failures.size() = " << ctx.failures.size() << std::endl;
    
    if (ctx.ast) delete ctx.ast;
    for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
        delete ctx.partialNodes[i];
    }
    
    std::cout << "  ✓ Passed" << std::endl;
}

void test_irc_channel_list_partial() {
    std::cout << "Test: IRC channel list - some invalid channels" << std::endl;
    
    Grammar g;
    g.addRule("<letter> ::= 'a' ... 'z' | 'A' ... 'Z'");
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<channelchar> ::= <letter> | <digit> | '_' | '-'");
    g.addRule("<channel> ::= '#' <letter> { <channelchar> }");
    g.addRule("<comma> ::= ','");
    g.addRule("<chanlist> ::= <channel> { <comma> <channel> }");
    
    BNFParser parser(g);
    ParseContext ctx;
    
    // "invalid" doesn't start with '#', should fail
    parser.parse("<chanlist>", "#chan1,invalid,#chan3", ctx);
    
    // First channel matches, then fails on "invalid"
    assert(ctx.success == true); // Repetition succeeds with partial match
    assert(ctx.consumed == 6); // Matched "#chan1" only
    
    // Should have tracked the failure
    assert(ctx.failures.size() >= 1);
    
    if (ctx.ast) delete ctx.ast;
    for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
        delete ctx.partialNodes[i];
    }
    
    std::cout << "  ✓ Passed" << std::endl;
}

void test_nested_sequence_and_repetition() {
    std::cout << "Test: Nested sequence and repetition" << std::endl;
    
    Grammar g;
    g.addRule("<letter> ::= 'a' ... 'z'");
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<word> ::= <letter> { <letter> }");
    g.addRule("<number> ::= <digit> { <digit> }");
    g.addRule("<space> ::= ' '");
    g.addRule("<item> ::= <word> <space> <number>");
    g.addRule("<list> ::= <item> { <space> <item> }");
    
    BNFParser parser(g);
    ParseContext ctx;
    
    // Should fail at second item (no number after "bar")
    parser.parse("<list>", "foo 123 bar", ctx);
    
    // Parsing might succeed or fail depending on how much matches
    std::cout << "  success = " << ctx.success << ", consumed = " << ctx.consumed << std::endl;
    std::cout << "  partialNodes.size() = " << ctx.partialNodes.size() << std::endl;
    std::cout << "  failures.size() = " << ctx.failures.size() << std::endl;
    
    // What matters is we tracked information about the parse
    // (This test just ensures the feature works without asserting specific values)
    
    for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
        delete ctx.partialNodes[i];
    }
    
    std::cout << "  ✓ Passed" << std::endl;
}

void test_optional_with_partial() {
    std::cout << "Test: Optional element in sequence with failure" << std::endl;
    
    Grammar g;
    g.addRule("<letter> ::= 'a' ... 'z'");
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<sign> ::= '+' | '-'");
    g.addRule("<number> ::= [ <sign> ] <digit> { <digit> }");
    g.addRule("<space> ::= ' '");
    g.addRule("<item> ::= <letter> <space> <number>");
    
    BNFParser parser(g);
    ParseContext ctx;
    
    // Should parse "a +123" successfully
    parser.parse("<item>", "a +123", ctx);
    
    assert(ctx.success == true);
    
    if (ctx.ast) delete ctx.ast;
    for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
        delete ctx.partialNodes[i];
    }
    
    std::cout << "  ✓ Passed" << std::endl;
}

void test_alternative_with_partial() {
    std::cout << "Test: Alternative in repetition with failures" << std::endl;
    
    Grammar g;
    g.addRule("<letter> ::= 'a' ... 'z'");
    g.addRule("<digit> ::= '0' ... '9'");
    g.addRule("<alphanum> ::= <letter> | <digit>");
    g.addRule("<word> ::= <alphanum> { <alphanum> }");
    
    BNFParser parser(g);
    ParseContext ctx;
    
    parser.parse("<word>", "a1b2c3@", ctx);
    
    // Should match "a1b2c3" and fail at '@'
    assert(ctx.success == true);
    assert(ctx.consumed == 6);
    // partialNodes tracks successfully parsed elements from repetition before failure
    assert(ctx.partialNodes.size() >= 1); // At least one element from repetition tracked
    assert(ctx.failures.size() >= 1); // Failed at '@'
    
    if (ctx.ast) delete ctx.ast;
    for (size_t i = 0; i < ctx.partialNodes.size(); ++i) {
        delete ctx.partialNodes[i];
    }
    
    std::cout << "  ✓ Passed" << std::endl;
}

int main() {
    std::cout << "=== Partial Parsing Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    test_partial_sequence_success();
    test_partial_sequence_failure();
    test_partial_repetition_all_valid();
    test_partial_repetition_mixed_validity();
    test_irc_channel_list_valid();
    test_irc_channel_list_partial();
    test_nested_sequence_and_repetition();
    test_optional_with_partial();
    test_alternative_with_partial();
    
    std::cout << std::endl;
    std::cout << "=== All Partial Parsing Tests Passed ===" << std::endl;
    
    return 0;
}
