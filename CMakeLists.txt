# Root CMakeLists.txt - Top-level build configuration
# This follows modern CMake conventions using subdirectories for better modularity - 2026-04-09

cmake_minimum_required(VERSION 3.20)
project(space-invaders LANGUAGES CXX VERSION 0.1.0)

# Modern CMake project structure: libraries first, then executables, then tests
# This ensures proper dependency ordering: lib -> src -> tests - 2026-04-09
add_subdirectory(lib)    # Build libraries first (other targets depend on these)
add_subdirectory(src)    # Build main executable (may depend on libraries)
add_subdirectory(tests)  # Build tests last (depend on both libraries and executables)