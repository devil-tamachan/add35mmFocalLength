set(CMAKE_VERBOSE_MAKEFILE 1)
cmake_minimum_required(VERSION 2.8)
project(sdl_opengl_example)

list(APPEND CMAKE_MODULE_PATH  "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

IF(APPLE)
  find_library(AUDIO_UNIT AudioUnit)
  MARK_AS_ADVANCED(AUDIO_UNIT)
  SET(EXTRA_LIBS ${AUDIO_UNIT})
ENDIF(APPLE)


include_directories(${SDL2_INCLUDE_DIR})
include_directories(../../)
file(GLOB IMGUI_SOURCES ../../imgui*.cpp)
add_executable(sdl_opengl_example main.cpp imgui_impl_sdl.cpp sqlite3.c ${IMGUI_SOURCES})
target_link_libraries(sdl_opengl_example ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY} ${OPENGL_LIBRARIES} ${EXTRA_LIBS})