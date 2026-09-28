# fa18_recomp: the translated game (PORT.md, Stages A-C) on the native
# machine layer, with an SDL2 window. Separate from port/CMakeLists.txt so
# fa18_port's native-build rules stay unchanged.
#
#   cmake -S port/recomp -B build/recomp-cmake
#   cmake --build build/recomp-cmake --config Release
cmake_minimum_required(VERSION 3.16)
project(fa18_recomp LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
# Reuse the SDL2 checkout fa18_port already fetched, when present.
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../build/_deps/sdl2-src/CMakeLists.txt AND NOT FETCHCONTENT_SOURCE_DIR_SDL2)
    set(FETCHCONTENT_SOURCE_DIR_SDL2 ${CMAKE_CURRENT_SOURCE_DIR}/../build/_deps/sdl2-src)
endif()
FetchContent_Declare(SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.11
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(SDL2)

set(MUSASHI ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/musashi)
add_library(fa18_musashi STATIC
    ${MUSASHI}/m68kcpu.c ${MUSASHI}/m68kops.c ${MUSASHI}/m68kdasm.c ${MUSASHI}/softfloat/softfloat.c)
target_include_directories(fa18_musashi PUBLIC ${MUSASHI})

file(GLOB FA18_GENERATED CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.c)
add_executable(fa18_recomp
    recomp_main.c recomp_runtime.c
    ../machine/machine.c ../machine/blitter.c ../machine/display.c ../machine/input.c
    ${FA18_GENERATED})
target_include_directories(fa18_recomp PRIVATE . generated ../machine)
target_compile_definitions(fa18_recomp PRIVATE FA18_WITH_SDL SDL_MAIN_HANDLED _CRT_SECURE_NO_WARNINGS)
target_link_libraries(fa18_recomp PRIVATE fa18_musashi SDL2-static)
if(MSVC)
    target_compile_definitions(fa18_musashi PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(fa18_musashi PRIVATE /w)
    # Generated routines are large label-heavy functions.
    target_compile_options(fa18_recomp PRIVATE /bigobj)
endif()
