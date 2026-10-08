add_library(fa18_native_runtime OBJECT replay.c frame_capture.c ../game/native/files.c ../game/postflight_file_callers.c
    ../game/native/frontend.c ../game/native/storage.c ../game/native/menu.c ../game/native/flight.c ../game/native/records.c ../game/native/setup.c
    ../game/native/raster.c ../game/native/scene.c ../game/native/model.c ../game/native/model_state.c ../game/native/display.c ../game/native/cockpit_assets.c
    ../game/native/audio_assets.c ../game/native/audio.c ../game/sound_resources.c ../game/native/menu_start.c ../game/native/viewport.c
    ../game/input_display_setup.c ../game/render_page.c
    ../game/native/frame_tail.c ../game/hud_stream.c
    ../game/native/frame_labels.c ../game/flight_markers.c
    ../game/native/frame_markers.c ../game/native/control_effects.c
    ../game/record_control_actions.c ../game/corner_view.c
    ../game/native/stores.c ../game/hud_stores.c
    ../game/native/input.c ../game/pending_input.c ../game/input_events.c
    ../game/command_dispatch.c ../game/view_commands.c ../game/player_input.c
    ../game/native/clock.c ../game/main_loop_timers.c
    ../game/control_actions.c ../game/update_sequence.c ../game/context_publication.c
    ../game/scene_placements.c ../game/draw_stream.c ../game/active_planes.c
    ../game/projection.c ../game/projection_readouts.c ../game/circle.c
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
    ../game/post_input_tick.c ../game/post_input.c ../game/scene_bootstrap.c ../game/template_gates.c
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
    ../amiga/ilbm.c ../amiga/pcm_output.c ../amiga/host_compat.c ../amiga/guest_memory.c)
target_sources(fa18_native_runtime PRIVATE host_input.c ../amiga/host_keys.c)
add_executable(fa18_native main.c)
target_include_directories(fa18_native_runtime PUBLIC ../game ../amiga ../romfree)
target_compile_definitions(fa18_native_runtime PUBLIC FA18_NATIVE SDL_MAIN_HANDLED _CRT_SECURE_NO_WARNINGS)
target_link_libraries(fa18_native_runtime PUBLIC SDL2-static)
target_link_libraries(fa18_native PRIVATE fa18_native_runtime)
add_custom_command(TARGET fa18_native POST_BUILD
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/prune_build_artifacts.py --quiet
    COMMENT "Bound disposable replay and validation artifacts")
if(MSVC)
    target_compile_options(fa18_native_runtime PRIVATE /W4 /Gy)
    target_compile_options(fa18_native PRIVATE /W4 /Gy)
    target_link_options(fa18_native PRIVATE /OPT:REF "/MAP:${CMAKE_CURRENT_BINARY_DIR}/fa18_native.map")
else()
    target_compile_options(fa18_native_runtime PRIVATE -Wall -Wextra -ffunction-sections -fdata-sections)
    target_compile_options(fa18_native PRIVATE -Wall -Wextra -ffunction-sections -fdata-sections)
    target_link_options(fa18_native PRIVATE -Wl,--gc-sections "-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/fa18_native.map")
endif()
if(BUILD_TESTING AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf)
    add_test(NAME fa18_native_artifact_policy
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/test_prune_build_artifacts.py)
    add_test(NAME fa18_native_artifact_cleanup
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/prune_build_artifacts.py --quiet)
    set_tests_properties(fa18_native_artifact_cleanup PROPERTIES FIXTURES_CLEANUP native_artifacts)
    add_executable(fa18_native_postflight_schedule_test ../../tools/native/native_postflight_schedule_test.c)
    target_link_libraries(fa18_native_postflight_schedule_test PRIVATE fa18_native_runtime)
    add_executable(fa18_native_qualification_sequence_test ../../tools/native/native_qualification_sequence_test.c)
    target_link_libraries(fa18_native_qualification_sequence_test PRIVATE fa18_native_runtime)
    add_executable(fa18_native_mission_success_test ../../tools/native/native_mission_success_test.c
        ../../tools/native/mission_pilot.c)
    target_link_libraries(fa18_native_mission_success_test PRIVATE fa18_native_runtime)
    add_test(NAME fa18_native_mission_three_success
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mission_success.py
                --test $<TARGET_FILE:fa18_native_mission_success_test>
                --out ${CMAKE_CURRENT_BINARY_DIR}/mission-three-success-check)
    add_test(NAME fa18_native_mission_five_combat_reset
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mission_combat_reset.py
                --test $<TARGET_FILE:fa18_native_mission_success_test>
                --out ${CMAKE_CURRENT_BINARY_DIR}/mission-five-combat-reset-check)
    add_test(NAME fa18_native_mission_five_forced_return
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mission_forced_return.py
                --test $<TARGET_FILE:fa18_native_mission_success_test>
                --out ${CMAKE_CURRENT_BINARY_DIR}/mission-five-forced-return-check)
    add_test(NAME fa18_native_mission_five_objective
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mission_five_objective.py
                --test $<TARGET_FILE:fa18_native_mission_success_test>
                --out ${CMAKE_CURRENT_BINARY_DIR}/mission-five-objective-check)
    add_test(NAME fa18_native_mission_five_success
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mission_success.py
                --test $<TARGET_FILE:fa18_native_mission_success_test> --mode 5
                --out ${CMAKE_CURRENT_BINARY_DIR}/mission-five-success-check)
    foreach(mission_mode 3 5)
        add_test(NAME fa18_native_mission_${mission_mode}_sequence
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mission_success.py
                    --test $<TARGET_FILE:fa18_native_mission_success_test> --mode ${mission_mode} --sequence
                    --out ${CMAKE_CURRENT_BINARY_DIR}/mission-${mission_mode}-sequence-check)
        set_tests_properties(fa18_native_mission_${mission_mode}_sequence PROPERTIES RUN_SERIAL TRUE)
    endforeach()
    set_tests_properties(fa18_native_mission_three_success fa18_native_mission_five_combat_reset
        fa18_native_mission_five_forced_return fa18_native_mission_five_objective
        fa18_native_mission_five_success
        PROPERTIES RUN_SERIAL TRUE)
    add_test(NAME fa18_native_qualification_sequence
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_qualification_sequence.py
                --test $<TARGET_FILE:fa18_native_qualification_sequence_test>
                --out ${CMAKE_CURRENT_BINARY_DIR}/qualification-sequence-check)
    add_test(NAME fa18_native_qualification_new_pilot
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_qualification_sequence.py
                --test $<TARGET_FILE:fa18_native_qualification_sequence_test> --new-pilot
                --out ${CMAKE_CURRENT_BINARY_DIR}/qualification-new-pilot-check)
    foreach(result_kind four five ready)
        add_test(NAME fa18_native_postflight_${result_kind}
            COMMAND fa18_native_postflight_schedule_test
                ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
                ${CMAKE_CURRENT_BINARY_DIR}/postflight-${result_kind}-save
                ${CMAKE_CURRENT_BINARY_DIR}/postflight-${result_kind} ${result_kind})
    endforeach()
    add_executable(fa18_native_mode_two_test
        ../../tools/native/native_mode_two_test.c ../../tools/native/gun_discovery.c)
    target_link_libraries(fa18_native_mode_two_test PRIVATE fa18_native_runtime)
    add_test(NAME fa18_native_mode_two
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mode_two.py
                --test $<TARGET_FILE:fa18_native_mode_two_test> --mode 2
                --out ${CMAKE_CURRENT_BINARY_DIR}/mode-two-check)
    add_test(NAME fa18_native_mode_125
        COMMAND fa18_native_mode_two_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/mode-125-save
            ${CMAKE_CURRENT_BINARY_DIR}/mode-125 125)
    add_test(NAME fa18_native_callback_reset
        COMMAND fa18_native_mode_two_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/callback-reset-save
            ${CMAKE_CURRENT_BINARY_DIR}/callback-reset 125 1 callback)
    add_test(NAME fa18_native_mode_six
        COMMAND fa18_native_mode_two_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/mode-six-save
            ${CMAKE_CURRENT_BINARY_DIR}/mode-six 6)
    add_test(NAME fa18_native_mode_four
        COMMAND fa18_native_mode_two_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/mode-four-save
            ${CMAKE_CURRENT_BINARY_DIR}/mode-four 4)
    add_test(NAME fa18_native_smoothing_cancel
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mode_two.py
                --test $<TARGET_FILE:fa18_native_mode_two_test> --mode 4 --smoothing
                --out ${CMAKE_CURRENT_BINARY_DIR}/smoothing-cancel-check)
    add_test(NAME fa18_native_region_flight
        COMMAND fa18_native_mode_two_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/region-flight-save
            ${CMAKE_CURRENT_BINARY_DIR}/region-flight 4 1 flight)
    foreach(mode IN ITEMS 5 6 7 8)
        if(mode EQUAL 8)
            add_test(NAME fa18_native_combat_${mode}
                COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mode_two.py
                    --test $<TARGET_FILE:fa18_native_mode_two_test> --mode 8 --combat
                    --out ${CMAKE_CURRENT_BINARY_DIR}/combat-eight-check)
        else()
            add_test(NAME fa18_native_combat_${mode}
                COMMAND fa18_native_mode_two_test
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
                    ${CMAKE_CURRENT_BINARY_DIR}/combat-${mode}-save
                    ${CMAKE_CURRENT_BINARY_DIR}/combat-${mode} ${mode} 1 combat)
        endif()
    endforeach()
    add_test(NAME fa18_native_natural_outcome
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mode_two.py
                --test $<TARGET_FILE:fa18_native_mode_two_test> --mode 6 --outcome
                --out ${CMAKE_CURRENT_BINARY_DIR}/natural-outcome-check)
    add_test(NAME fa18_native_radar_hit
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mode_two.py
                --test $<TARGET_FILE:fa18_native_mode_two_test> --mode 8 --hit
                --out ${CMAKE_CURRENT_BINARY_DIR}/radar-hit-check)
    add_test(NAME fa18_native_radar_kill
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mode_two.py
                --test $<TARGET_FILE:fa18_native_mode_two_test> --mode 8 --kill
                --out ${CMAKE_CURRENT_BINARY_DIR}/radar-kill-check)
    add_test(NAME fa18_native_infrared_kill
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mode_two.py
                --test $<TARGET_FILE:fa18_native_mode_two_test> --mode 8 --kill --missile infrared
                --out ${CMAKE_CURRENT_BINARY_DIR}/infrared-kill-check)
    add_test(NAME fa18_native_gun_hit
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mode_two.py
                --test $<TARGET_FILE:fa18_native_mode_two_test> --mode 8 --hit --gun
                --out ${CMAKE_CURRENT_BINARY_DIR}/gun-hit-check)
    add_test(NAME fa18_native_gun_kill
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mode_two.py
                --test $<TARGET_FILE:fa18_native_mode_two_test> --mode 8 --kill --gun
                --out ${CMAKE_CURRENT_BINARY_DIR}/gun-kill-check)
    add_test(NAME fa18_native_mode_five
        COMMAND fa18_native_mode_two_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/mode-five-save
            ${CMAKE_CURRENT_BINARY_DIR}/mode-five 5)
    add_test(NAME fa18_native_mode_seven
        COMMAND fa18_native_mode_two_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/mode-seven-save
            ${CMAKE_CURRENT_BINARY_DIR}/mode-seven 7)
    add_test(NAME fa18_native_mode_eight
        COMMAND fa18_native_mode_two_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/mode-eight-save
            ${CMAKE_CURRENT_BINARY_DIR}/mode-eight 8)
    add_test(NAME fa18_native_ejection
        COMMAND fa18_native_mode_two_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/ejection-save
            ${CMAKE_CURRENT_BINARY_DIR}/ejection 8 1 eject)
    foreach(weapon RANGE 1 3)
        add_test(NAME fa18_native_weapon_${weapon}
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_mode_two.py
                --test $<TARGET_FILE:fa18_native_mode_two_test> --mode 8 --weapon ${weapon}
                --out ${CMAKE_CURRENT_BINARY_DIR}/weapon-${weapon}-check)
    endforeach()
    foreach(aircraft RANGE 1 2)
        add_test(NAME fa18_native_mode_three_aircraft_${aircraft}
            COMMAND fa18_native_mode_two_test
                ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
                ${CMAKE_CURRENT_BINARY_DIR}/mode-three-${aircraft}-save
                ${CMAKE_CURRENT_BINARY_DIR}/mode-three-${aircraft} 3 ${aircraft})
    endforeach()
    add_executable(fa18_native_countermeasures_test ../../tools/native/native_countermeasures_test.c)
    target_link_libraries(fa18_native_countermeasures_test PRIVATE fa18_native_runtime)
    add_test(NAME fa18_native_countermeasures
        COMMAND fa18_native_countermeasures_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/countermeasure-save
            ${CMAKE_CURRENT_BINARY_DIR}/countermeasure)
    add_executable(fa18_native_host_keys_test ../../tools/native/native_host_keys_test.c)
    target_link_libraries(fa18_native_host_keys_test PRIVATE fa18_native_runtime)
    add_test(NAME fa18_native_host_keys
        COMMAND fa18_native_host_keys_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_BINARY_DIR}/host-keys-save
            ${CMAKE_CURRENT_BINARY_DIR}/host-keys)
    add_executable(fa18_native_scene_exit_test ../../tools/native/native_scene_exit_test.c)
    target_link_libraries(fa18_native_scene_exit_test PRIVATE fa18_native_runtime)
    add_test(NAME fa18_native_scene_exit
        COMMAND fa18_native_scene_exit_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_SOURCE_DIR}/../../captures/native/demo01/input.fa18in
            ${CMAKE_CURRENT_BINARY_DIR}/scene-exit-save
            ${CMAKE_CURRENT_BINARY_DIR}/scene-exit)
    add_executable(fa18_native_record_expiry_test ../../tools/native/native_record_expiry_test.c)
    target_link_libraries(fa18_native_record_expiry_test PRIVATE fa18_native_runtime)
    add_test(NAME fa18_native_record_expiry
        COMMAND fa18_native_record_expiry_test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../local/media/fa18.adf
            ${CMAKE_CURRENT_SOURCE_DIR}/../../captures/native/demo01/input.fa18in
            ${CMAKE_CURRENT_BINARY_DIR}/record-expiry-save)
    add_test(NAME fa18_native_frame_body
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_frame_body.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_samples
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_samples.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_audio
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_audio.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_frontend
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_frontend.py
                --runner $<TARGET_FILE:fa18_native>
                --map ${CMAKE_CURRENT_BINARY_DIR}/fa18_native.map)
    add_test(NAME fa18_native_frame_times
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_frame_times.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_view_depth
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_view_depth.py
                --runner $<TARGET_FILE:fa18_native>
                --out ${CMAKE_CURRENT_BINARY_DIR}/view-depth-check)
    set_tests_properties(fa18_native_view_depth PROPERTIES RUN_SERIAL TRUE)
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
    add_test(NAME fa18_native_raster
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_raster.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_clock
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_clock.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_actions
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_actions.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_viewport
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_viewport.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_frame_tail
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_frame_tail.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_menu_start
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_menu_start.py
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
    add_test(NAME fa18_native_demo
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_demo.py
                --runner $<TARGET_FILE:fa18_native>
                --map ${CMAKE_CURRENT_BINARY_DIR}/fa18_native.map)
    add_test(NAME fa18_native_cockpit_assets
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_cockpit_assets.py
                --runner $<TARGET_FILE:fa18_native>)
    add_test(NAME fa18_native_sound_resources
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/native/check_sound_resources.py
                --runner $<TARGET_FILE:fa18_native>)
    get_property(native_tests DIRECTORY PROPERTY TESTS)
    list(REMOVE_ITEM native_tests fa18_native_artifact_cleanup)
    set_tests_properties(${native_tests} PROPERTIES FIXTURES_REQUIRED native_artifacts)
endif()
