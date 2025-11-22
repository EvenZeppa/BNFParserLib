// #ifndef BNFINTERPRETER_HPP
// #define BNFINTERPRETER_HPP

// #include "Grammar.hpp"
// #include "Expression.hpp"
// #include <string>
// #include <vector>
// #include <map>

// // Structure pour accumuler les résultats
// // Replace the old Value definition with this safer, explicit version.
// struct Value {
//     enum Type { STRING, LIST, OBJECT } type;
//     std::string str;
//     std::vector<Value> list;
//     std::map<std::string, Value> object;

//     Value() : type(STRING) {}

//     // Constructor from string
// 	Value(const std::string& s) : type(STRING), str(s) {}


// 	// Copy constructor
// 	Value(const Value& other) : type(other.type), str(other.str), list(other.list), object(other.object) {}


// 	// Copy assignment
// 	Value& operator=(const Value& other) {
// 		if (this == &other) return *this;

// 		type   = other.type;
// 		str    = other.str;
// 		list   = other.list;
// 		object = other.object;
// 		return *this;
// 	}

// };


// // Classe principale de l’interpréteur BNF
// class BNFInterpreter {
// public:
// 	BNFInterpreter(Grammar* g) : grammar(g) {}
// 	~BNFInterpreter() {}

// 	// Parse input according to ruleName, return number of bytes consumed or -1 if failure
// 	int parse(const std::string& ruleName, const std::string& input, Value& out);

// private:
// 	Grammar* grammar;

// 	// Fonctions internes d’interprétation selon le type d’expression
// 	int interpretExpr(Expression* expr, const std::string& input, size_t& offset, Value& out);
// 	int interpretSequence(Expression* seq, const std::string& input, size_t& offset, Value& out);
// 	int interpretAlternative(Expression* alt, const std::string& input, size_t& offset, Value& out);
// 	int interpretRepeat(Expression* rep, const std::string& input, size_t& offset, Value& out);
// 	int interpretOptional(Expression* opt, const std::string& input, size_t& offset, Value& out);
// 	int interpretSymbol(Expression* sym, const std::string& input, size_t& offset, Value& out);
// 	int interpretTerminal(Expression* term, const std::string& input, size_t& offset, Value& out);

// 	// Fusionne un élément sous une clé dans un Value existant
// 	void mergeIntoKey(Value& dest, const std::string& key, const Value& item);
// };

// #endif
