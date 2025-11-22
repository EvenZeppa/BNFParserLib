#include "BNFParserInternal.hpp"
#include <iostream>

Expression* BNFParserInternal::parseExpression() {
    return parseAlternative();
}

Expression* BNFParserInternal::parseAlternative() {
    Expression* left = parseSequence();
    if (!left) return 0;

    while (true) {
        Token t = tokenizer.peek();
        if (t.type != Token::TOK_PIPE) break;

        tokenizer.next(); // consommer '|'
        Expression* right = parseSequence();
        if (!right) {
            delete left;
            return 0;
        }

        Expression* alt = new Expression(Expression::EXPR_ALTERNATIVE);
        alt->children.push_back(left);
        alt->children.push_back(right);
        left = alt;
    }

    return left;
}

Expression* BNFParserInternal::parseSequence() {
    std::vector<Expression*> parts;
    while (true) {
        Expression* e = parseRepeatable();
        if (!e) break;
        parts.push_back(e);
    }

    if (parts.empty()) return 0;
    if (parts.size() == 1) return parts[0];

    Expression* seq = new Expression(Expression::EXPR_SEQUENCE);
    seq->children = parts;
    return seq;
}

Expression* BNFParserInternal::parsePrimary() {
    Token t = tokenizer.peek(); // ne pas avancer tout de suite

    if (t.type == Token::TOK_TERMINAL) {
        tokenizer.next(); // consommer
        Expression* e = new Expression(Expression::EXPR_TERMINAL);
        e->value = t.value;
        return e;
    }

    if (t.type == Token::TOK_SYMBOL) {
        tokenizer.next(); // consommer
        Expression* e = new Expression(Expression::EXPR_SYMBOL);
        e->value = t.value;
        return e;
    }

    // éventuellement gérer parenthèses pour grouping
    // if (t.type == Token::TOK_LPAREN) { ... }

    return 0;
}


Expression* BNFParserInternal::parseRepeatable() {
    Expression* e = parsePrimary();
    if (!e) return 0;

    Token t = tokenizer.peek();

    if (t.type == Token::TOK_LBRACE) { // { ... } => répétition
        tokenizer.next(); // consommer '{'
        Expression* inside = parseAlternative(); // parser ce qui est à l'intérieur
        Token close = tokenizer.next();
        if (close.type != Token::TOK_RBRACE) {
            delete e;
            if (inside) delete inside;
            std::cerr << "Syntax error: expected '}'\n";
            return 0;
        }
        Expression* rep = new Expression(Expression::EXPR_REPEAT);
        // Ajouter l'expression initiale + contenu à répéter
        rep->children.push_back(inside ? inside : e);
        return rep;
    }

    if (t.type == Token::TOK_LBRACKET) { // [ ... ] => optionnel
        tokenizer.next(); // consommer '['
        Expression* inside = parseAlternative();
        Token close = tokenizer.next();
        if (close.type != Token::TOK_RBRACKET) {
            delete e;
            if (inside) delete inside;
            std::cerr << "Syntax error: expected ']'\n";
            return 0;
        }
        Expression* opt = new Expression(Expression::EXPR_OPTIONAL);
        opt->children.push_back(inside ? inside : e);
        return opt;
    }

    return e;
}


