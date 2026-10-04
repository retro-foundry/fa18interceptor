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
# The recreated game source and its temporary glue (PORT.md, Stage D).
file(GLOB FA18_GAME CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../game/*.c
                                      ${CMAKE_CURRENT_SOURCE_DIR}/../game/glue/*.c)
file(GLOB FA18_OS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../os/*.c)
# The source-backed selector, packet and transform core is also used by the
# earlier typed port. Keep its implementation in one place.
set(FA18_MAP_PACKET_CORE
    ../map_packet_original_pass.c ../map_packet_static_data.c
    ../map_packet_pass_runner.c ../map_packet_pass_selector.c
    ../map_packet_directory.c ../map_packet_coordinate_setup.c
    ../map_packet_control_stream.c ../map_packet_wide_control_stream.c
    ../map_packet_low_filter.c ../map_packet_column_table.c
    ../map_packet_control_walker.c ../map_packet_record_stage.c
    ../map_packet_relative_offset.c ../map_detail_gate.c
    ../map_detail_fields.c ../map_detail_component_route.c
    ../map_packet_stage.c ../map_packet_selector.c ../map_packet_transform.c
    ../map_packet_depth_stage.c)
add_library(fa18_map_packet_core STATIC ${FA18_MAP_PACKET_CORE})
target_include_directories(fa18_map_packet_core PRIVATE ../)
add_library(fa18_runtime STATIC
    recomp_runtime.c recomp_ports.c
    ../amiga/rom_audit.c ../amiga/runtime_guard.c ../amiga/service_dispatch.c ../amiga/exec_lists.c ../amiga/exec_task_services.c
    loop_input.c ../machine/machine.c ../machine/bus.c ../machine/blitter.c ../machine/display.c ../machine/input.c
    ${FA18_GAME} ${FA18_OS} ${FA18_GENERATED})
target_include_directories(fa18_runtime PUBLIC . generated ../machine ../game ../game/glue ../os)
target_compile_definitions(fa18_runtime PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(fa18_runtime PUBLIC fa18_musashi fa18_map_packet_core)

add_executable(fa18_recomp recomp_main.c)
target_compile_definitions(fa18_recomp PRIVATE FA18_WITH_SDL SDL_MAIN_HANDLED _CRT_SECURE_NO_WARNINGS)
target_link_libraries(fa18_recomp PRIVATE fa18_runtime SDL2-static)

include(CTest)
if(BUILD_TESTING)
    add_executable(fa18_machine_startup_test ../../tools/amiga/machine_startup_test.c)
    target_link_libraries(fa18_machine_startup_test PRIVATE fa18_runtime)
    if(MSVC)
        target_compile_options(fa18_machine_startup_test PRIVATE /UNDEBUG)
    else()
        target_compile_options(fa18_machine_startup_test PRIVATE -UNDEBUG)
    endif()
    add_test(NAME fa18_clean_machine_startup COMMAND fa18_machine_startup_test)
endif()
if(MSVC)
    target_compile_definitions(fa18_musashi PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(fa18_musashi PRIVATE /w)
    # Generated routines are large label-heavy functions.
    target_compile_options(fa18_runtime PRIVATE /bigobj)
endif()
