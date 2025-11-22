#ifndef SIMPLE_TEST_FRAMEWORK_HPP
#define SIMPLE_TEST_FRAMEWORK_HPP

#include <iostream>
#include <string>

static int passed = 0;
static int failed = 0;

// Assertion de l'égalité
#define ASSERT_EQ(a,b) \
    do { \
        if ((a) != (b)) { \
            std::cout << "[FAIL] " << __FILE__ << ":" << __LINE__ \
                      << ": " << #a << " != " << #b \
                      << " (" << (a) << " vs " << (b) << ")" << std::endl; \
            ++failed; \
        } else { \
            ++passed; \
        } \
    } while(0)

// Assertion d'un booléen vrai
#define ASSERT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::cout << "[FAIL] " << __FILE__ << ":" << __LINE__ \
                      << ": ASSERT_TRUE(" << #cond << ") failed" << std::endl; \
            ++failed; \
        } else { \
            ++passed; \
        } \
    } while(0)

// Affiche le résumé
inline void printTestSummary() {
    std::cout << "\n=== Test summary ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
}

#endif
