add_executable(fa18_native main.c replay.c
    ../game/native/frontend.c ../game/native/storage.c ../game/native/menu.c ../game/native/flight.c ../game/native/records.c ../game/native/setup.c
    ../game/native/raster.c ../game/native/scene.c ../game/native/model.c ../game/native/display.c
    ../game/input_display_setup.c ../game/render_page.c
    ../game/native/input.c ../game/pending_input.c ../game/input_events.c
    ../game/command_dispatch.c ../game/view_commands.c ../game/player_input.c
    ../game/native/clock.c ../game/main_loop_timers.c
    ../game/control_actions.c ../game/update_sequence.c
    ../game/scene_placements.c ../game/draw_stream.c ../game/active_planes.c
    ../game/projection.c ../game/circle.c
    ../game/followup_placements.c ../game/grid_projection_packet.c
    ../game/model_strips.c ../game/vertex_tail.c ../game/history_projection.c
    ../game/cockpit_script.c ../game/faces.c
    ../game/flight_commands.c
    ../game/native/hud.c ../game/postflight_hud.c ../game/hud_readouts.c
    ../game/hud_marks.c ../game/view_marks.c
    ../game/hud_bars.c ../game/render_span.c
    ../game/postflight_variants.c ../game/message_line.c
    ../game/notify.c
    ../game/display_records.c ../game/render_polygon.c ../game/render_line.c
    ../game/render_buffers.c ../game/plot.c ../game/clip.c
    ../game/polygon_clip.c ../game/attitude.c ../game/stages.c
    ../game/matrix_route.c ../game/postflight_completion.c ../game/postflight_messages.c ../game/map_detail_component_route.c
    ../game/map_detail_fields.c ../game/map_detail_gate.c ../game/map_packet.c
    ../game/map_packet_column_table.c ../game/map_packet_control_stream.c ../game/map_packet_control_walker.c
    ../game/map_packet_coordinate_setup.c ../game/map_packet_depth_stage.c ../game/map_packet_directory.c
    ../game/map_packet_low_filter.c ../game/map_packet_original_pass.c ../game/map_packet_pass_runner.c
    ../game/map_packet_pass_selector.c ../game/map_packet_record_stage.c ../game/map_packet_relative_offset.c
    ../game/map_packet_selector.c ../game/map_packet_stage.c ../game/map_packet_static_data.c
    ../game/map_packet_transform.c ../game/map_packet_wide_control_stream.c
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
    add_test(NAME fa18_native_models
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_models.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_hud
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_hud.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_clock
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_clock.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_actions
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_actions.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_strips
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_strips.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_map_limits
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_map_limits.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_postflight_entry
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_postflight_entry.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_input
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_input.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_replay
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_replay.py
                --runner $<TARGET_FILE:fa18_native>)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../captures/native/qual_carrier_success/state.bin AND
       EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../local/system/kick13.rom)
        add_test(NAME fa18_native_game_input
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_game_input.py
                    --runner $<TARGET_FILE:fa18_native>)
    endif()
    add_test(NAME fa18_native_collision
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_collision.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_sequence_return
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_sequence_return.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_qualification
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_qualification.py
                --runner $<TARGET_FILE:fa18_native>)
endif()
