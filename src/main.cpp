#include "Expression.hpp"
#include "Grammar.hpp"
#include <string>
#include <map>
#include <iostream>

class BNFInterpreter {
private:
	Grammar* grammar;
public:
	std::map<std::string, Expression*> rules;

	BNFInterpreter(Grammar* g) : grammar(g) {}

	~BNFInterpreter() {
		// Supprime toutes les règles et leurs expressions
		std::map<std::string, Expression*>::iterator it;
		for (it = rules.begin(); it != rules.end(); ++it) {
			delete it->second;
		}
		rules.clear();
	}

	bool parse(const std::string& ruleName, const std::string& input, std::string& output) {
		Rule* rule = grammar->getRule(ruleName);
		if (!rule || !rule->rootExpr) return false;
		return parseExpression(rule->rootExpr, input, output);
	}

	// Fonction récursive qui parse une Expression sur tout l’input
	bool parseExpression(Expression* expr, const std::string& input, std::string& output) {
		if (!expr) return false;

		switch (expr->type) {  
			case Expression::EXPR_TERMINAL:  
			std::cout << "Trying to match terminal: '" << expr->value << "' with input: '" << input << "'\n";
				if (input.substr(0, expr->value.size()) == expr->value) {
					output = expr->value;
					return true;
				}

				return false;  

			case Expression::EXPR_SYMBOL: {  
				std::cout << "Expanding symbol: '" << expr->value << "'\n";
				std::string result;  
				Rule* r = grammar->getRule(expr->value);  
				if (!r) return false;  
				if (parseExpression(r->rootExpr, input, result)) {  
					output = result;  
					return true;  
				}  
				return false;  
			}  

			case Expression::EXPR_SEQUENCE: {  
				std::cout << "Trying to match sequence: \n";
				std::string accumulated;  
				std::string remaining = input;
				for (size_t i = 0; i < expr->children.size(); ++i) {  
					Expression* child = expr->children[i];  
					std::string part;  
					if (!parseExpression(child, remaining, part)) return false;  
					accumulated += part;  
					remaining = remaining.substr(part.size());  
				}  
				output = accumulated;  
				return true;  
			}  

			case Expression::EXPR_ALTERNATIVE: {
				std::cout << "Trying to match alternative: \n"; 
				for (size_t i = 0; i < expr->children.size(); ++i) {  
					Expression* child = expr->children[i];  
					std::string part;  
					if (parseExpression(child, input, part)) {  
						output = part;  
						return true;  
					}  
				} 
				return false;  
			}  

			case Expression::EXPR_OPTIONAL: {
				std::string part;
				if (parseExpression(expr->children[0], input, part)) {
					output = part;
				} else {
					output = ""; // rien n'est consommé
				}
				return true; // toujours réussi
			}

			case Expression::EXPR_REPEAT: {
				std::string remaining = input;
				std::string accumulated;
				std::string part;

				while (parseExpression(expr->children[0], remaining, part) && !part.empty()) {
					accumulated += part;
					remaining = remaining.substr(part.size());
				}

				output = accumulated;
				return true; // succès même si rien n'a été consommé
			}
		}  
		return false;  
	}  
};

// Exemple main
int main() {
	Grammar g;
	
	// Définir des règles simples
	g.addRule("<A> ::= 'A'");
	g.addRule("<B> ::= 'B'");
	g.addRule("<AB> ::= <A> <B>");

	// Definir des règles avec alternatives et répétitions
	g.addRule("<letter> ::= 'A' | 'B'");
	g.addRule("<nick> ::= <letter> { <letter> }");

	// Definir regles plus complexes
	g.addRule("<word> ::= <letter> { <letter> | <number> }");
	g.addRule("<number> ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'");
	g.addRule("<command> ::= <letter> { <letter> } | <number> <number> <number>");

	// Definir regels avec optionnels
	g.addRule("<optional_letter> ::= [ <letter> ] 'X'");
	g.addRule("<optional_number> ::= [ <number> ] 'Y'");

	BNFInterpreter interp(&g);

	std::string result;  
	if (interp.parse("<AB>", "AB", result)) {  
		std::cout << "Parsed: " << result << std::endl;  
	} else {  
		std::cout << "Parse failed" << std::endl;  
	}

	if (interp.parse("<nick>", "ABBA", result)) {  
		std::cout << "Parsed nick: " << result << std::endl;  
	} else {  
		std::cout << "Parse nick failed" << std::endl;  
	}

	if (interp.parse("<word>", "AAB", result)) {
		std::cout << "Parsed word: " << result << std::endl;
	} else {
		std::cout << "Parse word failed" << std::endl;
	}

	if (interp.parse("<command>", "AAB", result)) {
		std::cout << "Parsed command: " << result << std::endl;
	} else {
		std::cout << "Parse command failed" << std::endl;
	}

	if (interp.parse("<command>", "123", result)) {
		std::cout << "Parsed command: " << result << std::endl;
	} else {
		std::cout << "Parse command failed" << std::endl;
	}

	if (interp.parse("<optional_letter>", "AX", result)) {
		std::cout << "Parsed optional_letter: " << result << std::endl;
	} else {
		std::cout << "Parse optional_letter failed" << std::endl;
	}

	if (interp.parse("<optional_letter>", "X", result)) {
		std::cout << "Parsed optional_letter: " << result << std::endl;
	} else {
		std::cout << "Parse optional_letter failed" << std::endl;
	}

	if (interp.parse("<optional_number>", "6Y", result)) {
		std::cout << "Parsed optional_number: " << result << std::endl;
	} else {
		std::cout << "Parse optional_number failed" << std::endl;
	}

	return 0;

}

// #include <iostream>
// #include "Grammar.hpp"
// #include "BNFInterpreter.hpp"
// #include "Value.hpp"

// static void printValue(const Value& v, int indent = 0) {
//     std::string pad(indent*2, ' ');
//     if (v.type == Value::NONE) {
//         std::cout << pad << "(none)\n";
//     } else if (v.type == Value::STRING) {
//         std::cout << pad << "\"" << v.str << "\"\n";
//     } else if (v.type == Value::LIST) {
//         std::cout << pad << "[\n";
//         for (size_t i = 0; i < v.list.size(); ++i) printValue(v.list[i], indent+1);
//         std::cout << pad << "]\n";
//     } else if (v.type == Value::OBJECT) {
//         std::cout << pad << "{\n";
//         for (std::map<std::string, Value>::const_iterator it = v.object.begin();
//              it != v.object.end(); ++it)
//         {
//             std::cout << pad << "  \"" << it->first << "\": ";
//             if (it->second.type == Value::STRING) {
//                 std::cout << "\"" << it->second.str << "\"\n";
//             } else {
//                 std::cout << "\n";
//                 printValue(it->second, indent+2);
//             }
//         }
//         std::cout << pad << "}\n";
//     }
// }

// int main() {
//     Grammar g;
//     g.addRule("<letter> ::= 'A' | 'B' | 'C'");
//     g.addRule("<number> ::= '0' | '1' | '2'");
//     g.addRule("<nick> ::= <letter> { <letter> | <number> }");

//     BNFInterpreter interp(g);

//     std::string input = "AB1";
//     ParseResult r = interp.parse("nick", input);

//     std::cout << "success=" << r.success << " consumed=" << r.consumed << "\n";
//     printValue(r.data);

//     return 0;
// }
