/* Util.hpp
 * Authored by Liam Harvell.
 */
// NOTE:=======================================================================
// 1. clearConsole is supposed to let me see only the current substep logs.
// TODO:=======================================================================
// 1. make clearConsole actually work.
// 2. Make a function that calculates an SDL_FRect so I can pass only the func
// to the renderer.
// ============================================================================
#pragma once
#include <iostream>

struct Vec2 {
  float x, y;
};

inline static constexpr Vec2 Zero() { return {0.0f, 0.0f}; }
inline static constexpr Vec2 Up() { return {0.0f, -1.0f}; }
inline static constexpr Vec2 Down() { return {0.0f, 1.0f}; }
inline static constexpr Vec2 Left() { return {-1.0f, 0.0f}; }
inline static constexpr Vec2 Right() { return {1.0f, 0.0f}; }
inline static constexpr Vec2 One() { return {1.0f, 1.0f}; }

inline Vec2 operator*(const Vec2 a, const Vec2 b) {
  return Vec2{.x = a.x * b.x, .y = a.y * b.y};
}
inline Vec2 operator*(const Vec2 a, const float b) {
  return Vec2{.x = a.x * b, .y = a.y * b};
}
inline Vec2 operator/(const Vec2 a, const Vec2 b) {
  return Vec2{.x = a.x / b.x, .y = a.y / b.y};
}
inline Vec2 operator/(const Vec2 a, const float b) {
  return Vec2{.x = a.x / b, .y = a.y / b};
}
inline Vec2 operator+(const Vec2 a, const Vec2 b) {
  return Vec2{.x = a.x + b.x, .y = a.y + b.y};
}
inline Vec2 operator+(const Vec2 a, const float b) {
  return Vec2{.x = a.x + b, .y = a.y + b};
}
// inline Vec2& operator=(Vec2& a, const Vec2 b) {
//   a.x = b.x;
//   a.y = b.y;
//   return a;
// }
inline Vec2 &operator*=(Vec2 &a, const Vec2 b) {
  a = a * b;
  return a;
}
inline Vec2 &operator*=(Vec2 &a, const float b) {
  a = a * b;
  return a;
}
inline Vec2 &operator/=(Vec2 &a, const Vec2 b) {
  a = a / b;
  return a;
}
inline Vec2 &operator/=(Vec2 &a, const float b) {
  a = a / b;
  return a;
}
inline Vec2 &operator+=(Vec2 &a, const Vec2 b) {
  a = a + b;
  return a;
}
inline Vec2 &operator+=(Vec2 &a, const float b) {
  a = a + b;
  return a;
}

// This is pulled directly from
// https://en.cppreference.com/cpp/utility/variant/visit2 @ 2026-04-22 14:45
// A *Variadic template*. The ... is called a parameter pack.
// "This template accepts any number of type arguments by the name Ts"
// struct overloaded : Ts... "multiple inheritance" via pack expansion.
// "the struct [overloaded] ihnherits from every type in Ts simultaneously"
// but C++ templates are a compile-time code generation mechanism

// This idiom exists because C++ doesn't have a built-in way to create an
// overload set from multiple lambdas.

// I'm writing this again because I still couldn't explain it.
// Overload takes multiple lambdas and glues them together into one object that
// inherits all their operator() functions.
// This is used in Events.hpp only as of 2026-05-17 21:05
template <typename... Ts> struct Overload : Ts... {
  using Ts::operator()...;
};
template <typename... Ts> Overload(Ts...) -> Overload<Ts...>;

inline void clearConsole() {
  // \033[2J clears the screen, \033[H moves cursor to top-left
  std::cout << "\033[2J\033[H" << std::endl;
}
