#include <iostream>
#include "Grammar.hpp"
#include "BNFInterpreter.hpp"
#include "Value.hpp"

static void printValue(const Value& v, int indent = 0) {
    std::string pad(indent*2, ' ');
    if (v.type == Value::NONE) {
        std::cout << pad << "(none)\n";
    } else if (v.type == Value::STRING) {
        std::cout << pad << "\"" << v.str << "\"\n";
    } else if (v.type == Value::LIST) {
        std::cout << pad << "[\n";
        for (size_t i = 0; i < v.list.size(); ++i) printValue(v.list[i], indent+1);
        std::cout << pad << "]\n";
    } else if (v.type == Value::OBJECT) {
        std::cout << pad << "{\n";
        for (std::map<std::string, Value>::const_iterator it = v.object.begin();
             it != v.object.end(); ++it)
        {
            std::cout << pad << "  \"" << it->first << "\": ";
            if (it->second.type == Value::STRING) {
                std::cout << "\"" << it->second.str << "\"\n";
            } else {
                std::cout << "\n";
                printValue(it->second, indent+2);
            }
        }
        std::cout << pad << "}\n";
    }
}

int main() {
    Grammar g;
    g.addRule("<letter> ::= 'A' | 'B' | 'C'");
    g.addRule("<number> ::= '0' | '1' | '2'");
    g.addRule("<nick> ::= <letter> { <letter> | <number> }");

    BNFInterpreter interp(g);

    std::string input = "AB1";
    ParseResult r = interp.parse("nick", input);

    std::cout << "success=" << r.success << " consumed=" << r.consumed << "\n";
    printValue(r.data);

    return 0;
}
