# Root CMakeLists.txt - Top-level build configuration
# This follows modern CMake conventions using subdirectories for better modularity - 2026-04-09

cmake_minimum_required(VERSION 3.20)
project(space-invaders LANGUAGES CXX VERSION 0.1.0)

# Find SDL3 packages at root level - makes them available to all subdirectories
# This follows modern CMake practice where dependencies are found once at root - 2026-04-09
# IMPORTANT: Both lib/ and src/ need SDL3, so we find it once here rather than duplicating find_package calls
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

# Modern CMake project structure: libraries first, then executables, then tests
# This ensures proper dependency ordering: lib -> src -> tests - 2026-04-09
add_subdirectory(lib)    # Build libraries first (other targets depend on these)
add_subdirectory(src)    # Build main executable (may depend on libraries)
add_subdirectory(tests)  # Build tests last (depend on both libraries and executables)