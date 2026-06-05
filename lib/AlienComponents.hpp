#pragma once
#include "CoreComponents.hpp"

enum class AlienType { Squid, Ship, Crab, Octopus };

struct Alien {
  AlienType type;
  int score = 0;
  std::optional<Ticker> death_ticker;

  bool is_formation_alien() const { return type != AlienType::Ship; }
};

struct MysteryTicker {
  int tick_count = 0;
};
