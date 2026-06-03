#pragma once

#include <cstdint>

// NOTE: This adding of a type to an enum is called Scoped Enumeration.
// It explicitly tells the compiler to use this amount of memory.
// It lets you forward declare it in other files
enum class GameLayer : uint32_t {
  None = 0,
  Player = 1 << 0,
  Wall = 1 << 1,
  Enemy = 1 << 2,
  Projectile = 1 << 3,
  All = 0xFFFFFFF,
};

inline GameLayer operator|(const GameLayer a, const GameLayer b) {
  return static_cast<GameLayer>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline GameLayer operator&(const GameLayer a, const GameLayer b) {
  return static_cast<GameLayer>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}
inline GameLayer operator~(const GameLayer a) {
  return static_cast<GameLayer>(~static_cast<uint32_t>(a));
}
inline GameLayer operator^(const GameLayer a, const GameLayer b) {
  return static_cast<GameLayer>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
}
inline GameLayer &operator|=(GameLayer &a, const GameLayer b) {
  a = a | b;
  return a;
}
inline GameLayer &operator&=(GameLayer &a, const GameLayer b) {
  a = a & b;
  return a;
}
inline GameLayer &operator^=(GameLayer &a, const GameLayer b) {
  a = a ^ b;
  return a;
}
