#pragma once
#include <iostream>

inline void clearConsole() {
  // \033[2J clears the screen, \033[H moves cursor to top-left
  std::cout << "\033[2J\033[H" << std::endl;
}
