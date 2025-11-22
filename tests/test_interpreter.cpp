#include "UnitTest.hpp"
#include "Grammar.hpp"
#include "BNFInterpreter.hpp"
#include <iostream>
#include <string>

static void assertString(const Value& v, const std::string& expected) {
	ASSERT_EQ(v.type, Value::STRING);
	ASSERT_EQ(v.str, expected);
}

static void assertListOfStrings(const Value& list, const std::vector<std::string>& expected) {
	ASSERT_EQ(list.type, Value::LIST);
	ASSERT_EQ(list.list.size(), expected.size());
	for (size_t i = 0; i < expected.size(); ++i) {
		ASSERT_EQ(list.list[i].type, Value::STRING);
		ASSERT_EQ(list.list[i].str, expected[i]);
	}
}

static void assertObjectHasKey(const Value& obj, const std::string& key) {
	ASSERT_EQ(obj.type, Value::OBJECT);
	ASSERT_TRUE(obj.object.find(key) != obj.object.end());
}

static const Value& getKey(const Value& obj, const std::string& key) {
	ASSERT_EQ(obj.type, Value::OBJECT);
	ASSERT_TRUE(obj.object.find(key) != obj.object.end());
	return obj.object.find(key)->second;
}

void test_interpreter_simple() {
	std::cout << "Running test_interpreter_simple...\n";

	Grammar g;
	g.addRule("<letter> ::= 'A' | 'B'");
	g.addRule("<nick> ::= <letter> { <letter> }");

	BNFInterpreter interp(&g);
	Value data;
	int consumed = interp.parse("nick", "AB", data);
	ASSERT_EQ(consumed, 2);

	assertObjectHasKey(data, "nick");
	assertString(getKey(data, "nick"), "AB");

	assertObjectHasKey(data, "letter");
	std::vector<std::string> expectedLetters;
	expectedLetters.push_back("A");
	expectedLetters.push_back("B");
	assertListOfStrings(getKey(data, "letter"), expectedLetters);
}

void test_interpreter_fail() {
	std::cout << "Running test_interpreter_fail...\n";

	Grammar g;
	g.addRule("<letter> ::= 'A' | 'B'");
	g.addRule("<nick> ::= <letter> { <letter> }");

	BNFInterpreter interp(&g);
	Value data;
	int consumed = interp.parse("nick", "CXX", data);
	ASSERT_EQ(consumed, -1);
}

void test_interpreter_repeat() {
	std::cout << "Running test_interpreter_repeat...\n";

	Grammar g;
	g.addRule("<letter> ::= 'A' | 'B'");
	g.addRule("<word> ::= <letter> { <letter> }");

	BNFInterpreter interp(&g);
	Value data;
	int consumed = interp.parse("word", "ABBA", data);

	ASSERT_EQ(consumed, 4);

	assertObjectHasKey(data, "letter");
	std::vector<std::string> expectedLetters;
	expectedLetters.push_back("A");
	expectedLetters.push_back("B");
	expectedLetters.push_back("B");
	expectedLetters.push_back("A");
	assertListOfStrings(getKey(data, "letter"), expectedLetters);
}

int main() {
	test_interpreter_simple();
	test_interpreter_fail();
	test_interpreter_repeat();

	printTestSummary();
	return (failed == 0) ? 0 : 1;
}
