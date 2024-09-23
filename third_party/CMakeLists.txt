add_subdirectory(stb)
add_subdirectory(glm)

## json
option(JSON_MultipleHeaders "Use non-amalgamated version of the library." ON)
option(JSON_Install         "Install CMake targets during install step." OFF)
add_subdirectory(json)

## CLI11
add_subdirectory(CLI11)

