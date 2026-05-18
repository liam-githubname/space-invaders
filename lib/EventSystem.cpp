/* EventSystem.cpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 1. std::visit combined with std::variant gives you polymorphism without
// pointers, exhaustiveness checking, and zero overhead.
// 2. According to AI there is a common idiom for handling this. the Overload
// pattern
// 3. In C++, every lambda expression creates a unique, anonymous struct type
// with an operator() member
// 4. Operator overloading is a complicated subset of c++, look at
// "operators in c++" note in obsidian vault.
// Using a generic Lambda expression with the if constexpr
// 5. The "if constexpr" forces the compiler to only generate only the
// matching branch is compiled for each event type. The others will be
// discarded at compile time
// TODO:=======================================================================
// 3. I need to figure out if GameState.hpp should be included or not.
// FIXME:======================================================================
// ============================================================================

#include "EventSystem.hpp"
// TODO: Remove after the logging isn't required
#include <cstdlib>

class EventSystem;

// This is the main use of this class here.
