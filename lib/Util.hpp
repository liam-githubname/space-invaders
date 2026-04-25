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
#include "GameState.hpp"
#include <iostream>

struct SDL_FRect;

inline void clearConsole() {
  // \033[2J clears the screen, \033[H moves cursor to top-left
  std::cout << "\033[2J\033[H" << std::endl;
}
