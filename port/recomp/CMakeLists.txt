# fa18_recomp: the translated game (PORT.md, Stages A-C) on the native
# machine layer, with an SDL2 window. Active game sources live in port/game.
#
#   cmake -S port/recomp -B build/recomp-cmake
#   cmake --build build/recomp-cmake --config Release
cmake_minimum_required(VERSION 3.16)
project(fa18_recomp LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
include(CTest)
add_subdirectory(../amiga ${CMAKE_CURRENT_BINARY_DIR}/amiga-compat)

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

# Native frontend explicitly requested 2026-10-06. Same game sources, independent
# executable: its link has no Musashi, translation, glue or machine objects.
option(FA18_NATIVE_ONLY "Build only the native intro/menu runner" OFF)
add_subdirectory(../native ${CMAKE_CURRENT_BINARY_DIR}/native)
if(FA18_NATIVE_ONLY)
    return()
endif()

set(MUSASHI ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/musashi)
add_library(fa18_musashi STATIC
    ${MUSASHI}/m68kcpu.c ${MUSASHI}/m68kops.c ${MUSASHI}/m68kdasm.c ${MUSASHI}/softfloat/softfloat.c)
target_include_directories(fa18_musashi PUBLIC ${MUSASHI})

file(GLOB FA18_GENERATED CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.c)
# The recreated game source and its temporary glue (PORT.md, Stage D).
file(GLOB FA18_GAME CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../game/*.c
                                      ${CMAKE_CURRENT_SOURCE_DIR}/../game/glue/*.c)
# The reusable loader library compiles these shared sources once.
get_filename_component(FA18_GAME_SOURCE_DIR ../game ABSOLUTE)
list(REMOVE_ITEM FA18_GAME ${FA18_GAME_SOURCE_DIR}/disk.c ${FA18_GAME_SOURCE_DIR}/hunk.c)
file(GLOB FA18_OS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../os/*.c)
add_library(fa18_runtime STATIC
    ../romfree/media.c
    recomp_runtime.c recomp_ports.c
    loop_input.c ../machine/machine.c ../machine/bus.c ../machine/blitter.c ../machine/display.c ../machine/input.c
    ${FA18_GAME} ${FA18_OS} ${FA18_GENERATED})
target_include_directories(fa18_runtime PUBLIC . generated ../machine ../game ../game/glue ../os)
target_compile_definitions(fa18_runtime PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(fa18_runtime PUBLIC fa18_musashi amiga_compat)
add_custom_target(fa18_static_recomp_check
    COMMAND ${Python3_EXECUTABLE} tools/recomp/static_recomp.py --check
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../..
    VERBATIM)
add_dependencies(fa18_runtime fa18_static_recomp_check)

add_executable(fa18_recomp recomp_main.c)
target_compile_definitions(fa18_recomp PRIVATE FA18_WITH_SDL SDL_MAIN_HANDLED _CRT_SECURE_NO_WARNINGS)
target_link_libraries(fa18_recomp PRIVATE fa18_runtime SDL2-static)
add_executable(fa18_romfree recomp_main.c ../romfree/profile.c)
target_compile_definitions(fa18_romfree PRIVATE FA18_ROMFREE_MAIN FA18_WITH_SDL SDL_MAIN_HANDLED _CRT_SECURE_NO_WARNINGS)
target_link_libraries(fa18_romfree PRIVATE fa18_runtime SDL2-static)

if(BUILD_TESTING)
    add_executable(fa18_host_alert_test ../../tools/amiga/host_alert_test.c)
    target_link_libraries(fa18_host_alert_test PRIVATE fa18_runtime)
    add_test(NAME fa18_host_alert_contract COMMAND fa18_host_alert_test)
    add_executable(fa18_romfree_startup_failure_test ../../tools/amiga/romfree_startup_failure_test.c)
    target_link_libraries(fa18_romfree_startup_failure_test PRIVATE fa18_runtime)
    add_executable(fa18_frame_pacer_test ../../tools/amiga/frame_pacer_test.c)
    add_test(NAME fa18_host_frame_pacing COMMAND fa18_frame_pacer_test)
    add_executable(fa18_host_gameport_test ../../tools/amiga/host_gameport_test.c)
    target_link_libraries(fa18_host_gameport_test PRIVATE fa18_runtime)
    add_test(NAME fa18_host_gameport_contract COMMAND fa18_host_gameport_test)
    add_executable(fa18_window_shutdown_test ../../tools/amiga/window_shutdown_test.c ../romfree/profile.c)
    target_compile_definitions(fa18_window_shutdown_test PRIVATE FA18_ROMFREE_MAIN FA18_WITH_SDL SDL_MAIN_HANDLED _CRT_SECURE_NO_WARNINGS)
    target_link_libraries(fa18_window_shutdown_test PRIVATE fa18_runtime SDL2-static)
    add_executable(fa18_machine_startup_test ../../tools/amiga/machine_startup_test.c)
    target_link_libraries(fa18_machine_startup_test PRIVATE fa18_runtime)
    if(MSVC)
        target_compile_options(fa18_machine_startup_test PRIVATE /UNDEBUG)
    else()
        target_compile_options(fa18_machine_startup_test PRIVATE -UNDEBUG)
    endif()
    add_test(NAME fa18_clean_machine_startup COMMAND fa18_machine_startup_test)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../captures/native/demo01/state.bin AND
       EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../local/system/kick13.rom)
        add_test(NAME fa18_emulation_meter_visibility
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/recomp/check_emulation_meter.py
                    --runner $<TARGET_FILE:fa18_recomp>)
    endif()
endif()
if(MSVC)
    target_compile_definitions(fa18_musashi PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(fa18_musashi PRIVATE /w)
    # Generated routines are large label-heavy functions.
    target_compile_options(fa18_runtime PRIVATE /bigobj)
endif()
