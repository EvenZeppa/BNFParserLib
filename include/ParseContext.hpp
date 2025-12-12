#ifndef PARSE_CONTEXT_HPP
#define PARSE_CONTEXT_HPP

#include "AST.hpp"
#include <string>
#include <vector>

/**
 * @brief Represents a failed parsing attempt for a sub-element.
 * 
 * When parsing a sequence or repetition, individual elements may fail.
 * This structure captures information about each failure to aid debugging
 * and error reporting.
 */
struct FailedNode {
    /**
     * @brief Position in input where this element failed to parse.
     */
    size_t position;

    /**
     * @brief The text fragment that failed to parse.
     * 
     * May be empty if failure occurred at end of input.
     */
    std::string text;

    /**
     * @brief Description of what was expected at this position.
     * 
     * Examples: "symbol <channel>", "terminal '#'", "character in range 'a'..'z'"
     */
    std::string expected;

    /**
     * @brief Name of the rule or symbol being parsed when failure occurred.
     */
    std::string ruleName;

    /**
     * @brief Constructs a FailedNode with all fields.
     */
    FailedNode(size_t pos, const std::string& txt, const std::string& exp, const std::string& rule)
        : position(pos), text(txt), expected(exp), ruleName(rule) {}
};

/**
 * @brief Context structure containing all parsing information.
 * 
 * This structure unifies parsing results, error information, and debugging
 * data into a single object. After parsing, users can check success status,
 * retrieve the AST, and get detailed error information about parse failures.
 * 
 * Benefits:
 *   - Single function call for parsing (no separate parseWithErrors)
 *   - Comprehensive error reporting with position and expected symbols
 *   - Tracks furthest failure position for better diagnostics
 *   - Cleaner API - all information in one place
 */
struct ParseContext {
    /**
     * @brief The root AST node if parsing succeeded, nullptr otherwise.
     * 
     * Caller is responsible for deleting this node when finished.
     * Only valid if success == true.
     */
    ASTNode* ast;

    /**
     * @brief Number of characters successfully consumed from input.
     * 
     * For successful parses, this is the length of the matched text.
     * For failures, indicates how far parsing progressed before failing.
     */
    size_t consumed;

    /**
     * @brief Position in input where parsing failed.
     * 
     * Records the furthest position reached during parsing attempts.
     * Useful for pinpointing where the input diverged from grammar expectations.
     * Only meaningful when success == false.
     */
    size_t errorPos;

    /**
     * @brief Description of what was expected at errorPos.
     * 
     * Human-readable string describing the symbol, terminal, or character
     * that was expected but not found. Examples:
     *   - "terminal 'hello'"
     *   - "symbol <digit>"
     *   - "character in range 'a'..'z'"
     *   - "one of: <digit> | <letter>"
     * 
     * Only meaningful when success == false.
     */
    std::string expected;

    /**
     * @brief True if parsing succeeded completely, false otherwise.
     * 
     * When true: ast is valid and consumed indicates matched length.
     * When false: ast is nullptr, errorPos and expected provide diagnostic info.
     */
    bool success;

    /**
     * @brief All successfully parsed sub-elements, even if overall parse failed.
     * 
     * For partial parsing support: contains AST nodes for each successfully
     * parsed element in a sequence or repetition, even if subsequent elements
     * failed. Enables recovery of valid data from partially valid inputs.
     * 
     * Example: parsing "JOIN #chan1,invalid,#chan3" might populate:
     *   - partialNodes[0]: AST for "#chan1"
     *   - partialNodes[1]: AST for "#chan3"
     * 
     * Caller is responsible for deleting these nodes when finished.
     */
    std::vector<ASTNode*> partialNodes;

    /**
     * @brief All failed parsing attempts for sub-elements.
     * 
     * Records each sub-element that failed to parse, with position, text,
     * and expected symbol information. Useful for detailed error reporting
     * and understanding what went wrong in complex inputs.
     * 
     * Example: parsing "JOIN #chan1,invalid,#chan3" might populate:
     *   - failures[0]: {pos: 12, text: "invalid", expected: "symbol <channel>"}
     */
    std::vector<FailedNode> failures;

    /**
     * @brief Constructs an empty ParseContext with default values.
     */
    ParseContext() 
        : ast(0), consumed(0), errorPos(0), expected(""), success(false) {}

    /**
     * @brief Resets the context to initial state.
     * 
     * Note: Does NOT delete the AST nodes - caller must manage AST memory.
     */
    void reset() {
        ast = 0;
        consumed = 0;
        errorPos = 0;
        expected = "";
        success = false;
        partialNodes.clear();
        failures.clear();
    }

    /**
     * @brief Updates error information if this position is further than current.
     * 
     * Implements "furthest failure" tracking - only updates error info when
     * a new failure occurs at a position beyond the current errorPos.
     * 
     * @param pos Position where failure occurred
     * @param expectedSymbol Description of what was expected
     */
    void updateError(size_t pos, const std::string& expectedSymbol) {
        if (pos > errorPos) {
            errorPos = pos;
            expected = expectedSymbol;
        } else if (pos == errorPos && !expectedSymbol.empty()) {
            // At same position, accumulate alternatives
            if (!expected.empty() && expected.find(expectedSymbol) == std::string::npos) {
                expected += " | " + expectedSymbol;
            } else if (expected.empty()) {
                expected = expectedSymbol;
            }
        }
    }
};

#endif // PARSE_CONTEXT_HPP
