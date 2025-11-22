#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <sstream>

// #define ENABLE_DEBUG

/*
 * Simple debug macros. Define ENABLE_DEBUG (e.g. -DENABLE_DEBUG) to enable
 * debug output. Macros are C++98 friendly and use std::ostringstream to
 * evaluate streamable values safely.
 */
#ifdef ENABLE_DEBUG

#define DEBUG_MSG(msg) \
    do { \
        std::ostringstream __dbg_ss; \
        __dbg_ss << msg; \
        std::cerr << "[DEBUG] " << __dbg_ss.str() << std::endl; \
    } while(0)

#define DEBUG_VAL(name, val) \
    do { \
        std::ostringstream __dbg_ss; \
        __dbg_ss << (val); \
        std::cerr << "[DEBUG] " << (name) << "=" << __dbg_ss.str() << std::endl; \
    } while(0)

#else

#define DEBUG_MSG(msg) do { } while(0)
#define DEBUG_VAL(name, val) do { } while(0)

#endif // ENABLE_DEBUG

#endif // DEBUG_HPP
