#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>

/**
 * @brief Represents a grammar expression node used by the interpreter.
 *
 * The Expression structure models different grammar constructs (sequence,
 * alternative, optional, repetition, symbol, terminal). Nodes may have
 * child expressions for composite constructs.
 */
struct Expression {
    /**
     * @brief Type of the expression node.
     *
     * - EXPR_SEQUENCE: a sequence of child expressions evaluated in order.
     * - EXPR_ALTERNATIVE: a choice between child expressions.
     * - EXPR_OPTIONAL: an optional child expression.
     * - EXPR_REPEAT: a repeating child expression (zero or more).
     * - EXPR_SYMBOL: a non-terminal symbol reference.
     * - EXPR_TERMINAL: a terminal token/value.
     */
    enum Type {
        EXPR_SEQUENCE,
        EXPR_ALTERNATIVE,
        EXPR_OPTIONAL,
        EXPR_REPEAT,
        EXPR_SYMBOL,
        EXPR_TERMINAL
    };

    // The node type.
    Type type;
    // Child expressions (used for composite types like sequence/alternative).
    std::vector<Expression*> children;
    // Optional textual value (e.g. symbol name or terminal text).
    std::string value;

    /**
     * @brief Constructs an Expression of the given type.
     *
     * @param t Type of expression to create.
     */
    Expression(Type t);

    /**
     * @brief Destructor. Responsible for cleaning up owned children.
     */
    ~Expression();
};

#endif
