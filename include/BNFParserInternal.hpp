// src/BNFParserInternal.hpp
#ifndef BNF_PARSER_INTERNAL_HPP
#define BNF_PARSER_INTERNAL_HPP

#include "Expression.hpp"
#include "BNFTokenizer.hpp"
#include <vector>

class BNFParserInternal {
public:
    BNFParserInternal(BNFTokenizer& tz) : tokenizer(tz) {}

    Expression* parseExpression();
private:
    Expression* parseAlternative();
    Expression* parseSequence();
    Expression* parseRepeatable();
    Expression* parsePrimary();

    BNFTokenizer& tokenizer;
};

#endif
