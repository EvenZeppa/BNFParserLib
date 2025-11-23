#include "Expression.hpp"
#include "Debug.hpp"

// Expression implementation
Expression::Expression(Type t)
    : type(t) {
    DEBUG_MSG("Expression created: type=" << t);
}

// Destructor recursively deletes all child expressions to prevent memory leaks
Expression::~Expression() {
    DEBUG_MSG("Expression destroyed: type=" << type << " with " << children.size() << " children");
    for (size_t i = 0; i < children.size(); ++i)
        delete children[i];
    children.clear();
}
