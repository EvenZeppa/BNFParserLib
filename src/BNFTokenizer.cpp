#include "BNFTokenizer.hpp"
#include "Debug.hpp"
#include <cctype>

// Token implementation
Token::Token(Type t, const std::string& v)
    : type(t), value(v) {}

// BNFTokenizer implementation
BNFTokenizer::BNFTokenizer(const std::string& input)
    : text(input), pos(0) {}

// Skip whitespace characters (space and tab) at current position
void BNFTokenizer::skipSpaces() {
    while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\t'))
        ++pos;
}

// Look ahead at next token without consuming it
Token BNFTokenizer::peek() {
    size_t save = pos;
    Token t = next();
    pos = save;
    return t;
}

Token BNFTokenizer::next() {
    skipSpaces();

    if (pos >= text.size()) {
        DEBUG_MSG("BNFTokenizer::next: reached end of input");
        return Token(Token::TOK_END, "");
    }

    char c = text[pos];
    DEBUG_MSG("BNFTokenizer::next: parsing char '" << std::string(1, c) << "' at pos=" << pos);

    // Symbol <...>
    if (c == '<')
        return parseSymbol();

    // Terminal '...' or "..."
    if (c == '\'' || c == '"')
        return parseTerminal();

    // Single-character tokens
    if (c == '{') { pos++; DEBUG_MSG("BNFTokenizer::next: found LBRACE"); return Token(Token::TOK_LBRACE, "{"); }
    if (c == '}') { pos++; DEBUG_MSG("BNFTokenizer::next: found RBRACE"); return Token(Token::TOK_RBRACE, "}"); }
    if (c == '[') { pos++; DEBUG_MSG("BNFTokenizer::next: found LBRACKET"); return Token(Token::TOK_LBRACKET, "["); }
    if (c == ']') { pos++; DEBUG_MSG("BNFTokenizer::next: found RBRACKET"); return Token(Token::TOK_RBRACKET, "]"); }
    if (c == '|') { pos++; DEBUG_MSG("BNFTokenizer::next: found PIPE"); return Token(Token::TOK_PIPE, "|"); }

    // Word (fallback)
    return parseWord();
}

// Parse a symbol token of the form <name>, including angle brackets
Token BNFTokenizer::parseSymbol() {
    size_t start = pos++;
    while (pos < text.size() && text[pos] != '>')
        pos++;
    if (pos < text.size()) pos++; // include '>'
    std::string symbol = text.substr(start, pos - start);
    DEBUG_MSG("BNFTokenizer::parseSymbol: found symbol '" << symbol << "'");
    return Token(Token::TOK_SYMBOL, symbol);
}

// Parse a terminal token enclosed in quotes, returning content without quotes
Token BNFTokenizer::parseTerminal() {
    char quote = text[pos];
    size_t start = ++pos; // start after opening quote
    while (pos < text.size() && text[pos] != quote)
        pos++;
    std::string val = text.substr(start, pos - start); // content without quotes
    if (pos < text.size()) pos++; // consume closing quote
    DEBUG_MSG("BNFTokenizer::parseTerminal: found terminal '" << val << "'");
    return Token(Token::TOK_TERMINAL, val);
}

// Parse a simple word token, stopping at whitespace or special characters
Token BNFTokenizer::parseWord() {
    size_t start = pos;
    while (pos < text.size() &&
           !isspace(text[pos]) &&
           text[pos] != '|' &&
           text[pos] != '{' && text[pos] != '}' &&
           text[pos] != '[' && text[pos] != ']')
    {
        pos++;
    }
    return Token(Token::TOK_WORD, text.substr(start, pos - start));
}
