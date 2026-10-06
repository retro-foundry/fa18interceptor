add_executable(fa18_native main.c
    ../game/native/frontend.c ../game/native/storage.c ../game/native/menu.c ../game/native/flight.c ../game/native/records.c ../game/native/setup.c
    ../game/main_loop_control_messages.c ../game/text.c ../game/menu_setup.c
    ../game/message_reset.c ../game/menu_available.c ../game/menu_table.c ../game/menu_transition.c
    ../game/command_selection.c ../game/indexed_commands.c ../game/numbers.c
    ../game/scene_dispatch.c ../game/scene_setup.c ../game/control_records.c
    ../game/matrix.c ../game/fixed_math.c ../game/tracking.c ../game/view.c
    ../game/stage_tables.c ../game/fault.c ../game/menu_cold.c
    ../game/context_refresh.c ../game/template_placements.c
    ../game/placement_order.c ../game/renderer_workspace.c ../game/audio.c ../game/view_transform.c ../game/messages.c
    ../game/post_input_tick.c ../game/scene_bootstrap.c ../game/template_gates.c
    ../game/input_device_callbacks.c ../game/menu_return.c ../game/menu_followup.c ../game/menu_outcome.c
    ../game/update_stage.c ../game/record_update_stage.c ../game/flight_record_actions.c
    ../game/flight_dynamics.c ../game/flight_recorder.c ../game/record_steering.c
    ../game/record_matrix_update.c ../game/indexed_record_update.c ../game/postflight_scheduler.c
    ../game/candidate_record_update.c ../game/candidate_record_scan.c
    ../game/candidate_level_walk.c
    ../game/flight_geometry.c ../game/control_input.c ../game/plane_tests.c
    ../game/flight_motion_helpers.c ../game/record_region_probe.c
    ../game/selector_origin.c
    ../game/main_loop_flight_controls.c
    ../game/command_publication.c ../game/context_commands.c ../game/cockpit.c
    ../game/menu_context_finish.c ../game/target_heading.c
    ../game/disk.c ../game/hunk.c ../romfree/media.c ../amiga/sha256.c
    ../amiga/ilbm.c)
target_include_directories(fa18_native PRIVATE ../game ../amiga ../romfree)
target_compile_definitions(fa18_native PRIVATE FA18_NATIVE SDL_MAIN_HANDLED _CRT_SECURE_NO_WARNINGS)
target_link_libraries(fa18_native PRIVATE SDL2-static)
if(MSVC)
    target_compile_options(fa18_native PRIVATE /W4 /Gy)
    target_link_options(fa18_native PRIVATE /OPT:REF "/MAP:${CMAKE_CURRENT_BINARY_DIR}/fa18_native.map")
else()
    target_compile_options(fa18_native PRIVATE -Wall -Wextra -ffunction-sections -fdata-sections)
    target_link_options(fa18_native PRIVATE -Wl,--gc-sections "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/fa18_native.map")
endif()
if(BUILD_TESTING AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf)
    add_test(NAME fa18_native_frontend
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_frontend.py
                --runner $<TARGET_FILE:fa18_native>
                --map ${CMAKE_CURRENT_BINARY_DIR}/fa18_native.map)
    add_test(NAME fa18_native_menu
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_menu.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_flight_start
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_flight_start.py
                --runner $<TARGET_FILE:fa18_native>)
endif()
