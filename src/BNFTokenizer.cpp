#include "BNFTokenizer.hpp"
#include <cctype>

Token::Token(Type t, const std::string& v)
    : type(t), value(v) {}

BNFTokenizer::BNFTokenizer(const std::string& input)
    : text(input), pos(0) {}

void BNFTokenizer::skipSpaces() {
    while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\t'))
        ++pos;
}

Token BNFTokenizer::peek() {
    size_t save = pos;
    Token t = next();
    pos = save;
    return t;
}

Token BNFTokenizer::next() {
    skipSpaces();

    if (pos >= text.size())
        return Token(Token::TOK_END, "");

    char c = text[pos];

    // Symbol <...>
    if (c == '<')
        return parseSymbol();

    // Terminal '...' or "..."
    if (c == '\'' || c == '"')
        return parseTerminal();

    // Single-character tokens
    if (c == '{') { pos++; return Token(Token::TOK_LBRACE, "{"); }
    if (c == '}') { pos++; return Token(Token::TOK_RBRACE, "}"); }
    if (c == '[') { pos++; return Token(Token::TOK_LBRACKET, "["); }
    if (c == ']') { pos++; return Token(Token::TOK_RBRACKET, "]"); }
    if (c == '|') { pos++; return Token(Token::TOK_PIPE, "|"); }

    // Word (fallback)
    return parseWord();
}

// #include <iostream>

// Token BNFTokenizer::next() {
//     skipSpaces();

//     if (pos >= text.size()) {
//         Token t(Token::TOK_END, "");
//         std::cout << "[Tokenizer] Token: type=TOK_END, value=''" << std::endl;
//         return t;
//     }

//     char c = text[pos];

//     Token t(Token::TOK_END, ""); // token par défaut, sera remplacé

//     // Symbol <...>
//     if (c == '<')
//         t = parseSymbol();
//     // Terminal '...' or "..."
//     else if (c == '\'' || c == '"')
//         t = parseTerminal();
//     // Single-character tokens
//     else if (c == '{') { pos++; t = Token(Token::TOK_LBRACE, "{"); }
//     else if (c == '}') { pos++; t = Token(Token::TOK_RBRACE, "}"); }
//     else if (c == '[') { pos++; t = Token(Token::TOK_LBRACKET, "["); }
//     else if (c == ']') { pos++; t = Token(Token::TOK_RBRACKET, "]"); }
//     else if (c == '|') { pos++; t = Token(Token::TOK_PIPE, "|"); }
//     // Word (fallback)
//     else
//         t = parseWord();

//     // Affichage pour debug
//     std::cout << "[Tokenizer] Token: type=" << t.type << ", value='" << t.value << "'" << std::endl;

//     return t;
// }

Token BNFTokenizer::parseSymbol() {
    size_t start = pos++;
    while (pos < text.size() && text[pos] != '>')
        pos++;
    if (pos < text.size()) pos++; // include '>'
    return Token(Token::TOK_SYMBOL, text.substr(start, pos - start));
}

Token BNFTokenizer::parseTerminal() {
    char quote = text[pos];
    size_t start = ++pos; // commencer après la quote
    while (pos < text.size() && text[pos] != quote)
        pos++;
    std::string val = text.substr(start, pos - start); // sans les quotes
    if (pos < text.size()) pos++; // consommer la quote fermante
    return Token(Token::TOK_TERMINAL, val);
}

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
