cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(third_party)

project(
  obj2psx
  VERSION 0.1.0
  LANGUAGES CXX
)

add_executable(obj2psx
  obj2psx/src/PsxModel.cpp
  obj2psx/src/Obj2PsxConverter.cpp
  obj2psx/src/ObjFile.cpp
  obj2psx/src/main.cpp
)

project(
  timtool
  VERSION 0.1.0
  LANGUAGES CXX
)

add_executable(timtool
  timtool/src/ImageLoader.cpp
  timtool/src/TimCreateConfig.cpp
  timtool/src/TimFileRead.cpp
  timtool/src/TimFileWrite.cpp
  timtool/src/TimFileCreator.cpp
  timtool/src/main.cpp
)

target_link_libraries(timtool PRIVATE
  stb::image
  nlohmann_json::nlohmann_json
)

# TODO: CMake 3.26 - use ImageMagick::Magick++
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
target_compile_definitions(timtool PRIVATE -DMAGICKCORE_QUANTUM_DEPTH=16 )
target_compile_definitions(timtool PRIVATE -DMAGICKCORE_HDRI_ENABLE=0 )
target_include_directories(timtool PRIVATE ${ImageMagick_INCLUDE_DIRS})
target_link_libraries(timtool PRIVATE ${ImageMagick_LIBRARIES})
