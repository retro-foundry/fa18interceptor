add_executable(fa18_native main.c
    ../game/native/frontend.c ../game/native/storage.c ../game/native/menu.c
    ../game/main_loop_control_messages.c ../game/text.c ../game/menu_setup.c
    ../game/message_reset.c ../game/menu_available.c ../game/menu_table.c ../game/menu_transition.c
    ../game/command_selection.c ../game/indexed_commands.c ../game/numbers.c
    ../game/disk.c ../game/hunk.c ../romfree/media.c ../amiga/sha256.c
    ../amiga/ilbm.c)
target_include_directories(fa18_native PRIVATE ../game ../amiga ../romfree)
target_compile_definitions(fa18_native PRIVATE FA18_NATIVE SDL_MAIN_HANDLED _CRT_SECURE_NO_WARNINGS)
target_link_libraries(fa18_native PRIVATE SDL2-static)
if(MSVC)
    target_compile_options(fa18_native PRIVATE /W4)
    target_link_options(fa18_native PRIVATE "/MAP:${CMAKE_CURRENT_BINARY_DIR}/fa18_native.map")
else()
    target_compile_options(fa18_native PRIVATE -Wall -Wextra)
    target_link_options(fa18_native PRIVATE "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/fa18_native.map")
endif()
if(BUILD_TESTING AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf)
    add_test(NAME fa18_native_frontend
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_frontend.py
                --runner $<TARGET_FILE:fa18_native>
                --map ${CMAKE_CURRENT_BINARY_DIR}/fa18_native.map)
    add_test(NAME fa18_native_menu
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_menu.py
                --runner $<TARGET_FILE:fa18_native>)
endif()
