/* Registry of recreated routines (recomp_ports.h). One row per original
 * routine: its address, glue, the C function it calls, and the CPU cycles to
 * charge in ON mode: instruction time only; blitter waits are charged by
 * wait_blitter(). */
#include "recomp_ports.h"

#include "ports_glue.h"

const FA18Port fa18_ports[] = {
    /* render_polygon.c */
    {0xC30466, glue_C30466, "composite_polygon_plane", 200},
    {0xC304B2, glue_C304B2, "clear_polygon_mask", 150},
    {0xC305AA, glue_C305AA, "draw_polygon_edge", 400},
    /* render_line.c */
    {0xC2FA7E, glue_C2FA7E, "draw_line", 700},
    /* fixed_math.c */
    {0xC2E6DA, glue_C2E6DA, "sin_cos", 120},
    /* audio.c */
    {0xC501E0, glue_C501E0, "set_voice_output", 110},
    {0xC24FE8, glue_C24FE8, "fade_master_volume", 60},
    /* text.c */
    {0xC330FE, glue_C330FE, "plot_glyph8", 300},
    {0xC32806, glue_C32806, "plot_glyph3", 300},
    /* numbers.c, input.c, render_page.c */
    {0xC25A08, glue_C25A08, "pack_display_value", 400},
    {0xC1715C, glue_C1715C, "read_mouse_buttons", 90},
    {0xC2F558, glue_C2F558, "select_draw_page", 90},
    /* notify.c */
    {0xC11B44, glue_C11B44, "tick_notification_cadence", 100},
    /* fixed_math.c */
    {0xC15138, glue_C15138, "attenuate_offset", 150},
    /* render_span.c */
    {0xC310E2, glue_C310E2, "bound_span", 80},
    /* control_records.c */
    {0xC1EBC0, glue_C1EBC0, "read_record_fields", 90},
    {0xC230B0, glue_C230B0, "release_lost_selection", 90},
    {0xC2DE96, glue_C2DE96, "settle_record", 50},
    /* screen_frame.c */
    {0xC0DAA0, glue_C0DAA0, "append_mirrored_points", 150},
    {0xC0DAD0, glue_C0DAD0, "append_point", 30},
    {0xC0DAD4, glue_C0DAD4, "append_point", 40},
    {0xC0DADC, glue_C0DADC, "append_point", 40},
    {0xC0DAE6, glue_C0DAE6, "append_point", 40},
    /* stages.c */
    {0xC25B1C, glue_empty_stage, "empty_stage", 16},
    {0xC25B1E, glue_empty_stage, "empty_stage", 16},
    {0xC31F4A, glue_empty_stage, "empty_stage", 16},
    {0xC25864, glue_C25864, "reset_list", 50},
    {0xC25482, glue_C25482, "tick_timer", 30},
    {0xC08394, glue_C08394, "set_event_bit_and_clear_command_word_bit", 48},
    {0xC090C2, glue_C090C2, "clear_scene_startup_state", 1200},
    {0xC090F2, glue_C090F2, "enable_scene_record_updates", 180},
    {0xC1B602, glue_C1B602, "reset_throttle_input_state", 42},
    {0xC0833E, glue_C0833E, "dispatch_space_command_effect", 110},
    {0xC133B2, glue_C133B2, "record_6e_step", 220},
    {0xC118A0, glue_C118A0, "queue_postflight_failure_message", 240},
    {0xC083E2, glue_C083E2, "begin_mission_reset", 1000},
    {0xC25A00, glue_C25A00, "add_repeated_nibble_weight", 150},
    {0xC30AE2, glue_C30AE2, "draw_stores_icon_stream", 500},
    {0xC30A00, glue_C30A00, "draw_stores_icons", 900},
    {0xC17B96, glue_C17B96, "start_menu_sound_pair", 600},
    {0xC2F1C0, glue_C2F1C0, "draw_filled_circle", 4000},
    {0xC2EC90, glue_C2EC90, "project_view_point", 550},
    {0xC2EC94, glue_C2EC94, "project_view_point_mode", 650},
    {0xC2EC9C, glue_C2EC9C, "project_view_point_mode", 650},
    {0xC2ECA4, glue_C2ECA4, "project_view_point_mode", 650},
    {0xC1FE24, glue_C1FE24, "draw_display_stream_point", 750},
    {0xC1FE46, glue_C1FE46, "draw_display_stream_point", 750},
    {0xC0DAEE, glue_C0DAEE, "draw_fixed_matrix_mark", 1100},
    {0xC17F8C, glue_C17F8C, "start_sound_6", 1100},
    {0xC18108, glue_C18108, "start_sound_12", 1050},
    {0xC1B906, glue_C1B906, "start_view_mode_zero", 900},
    {0xC0CFFA, glue_C0CFFA, "draw_scaled_view_circle", 1300},
    {0xC0CF98, glue_C0CF98, "draw_scaled_stream_circle", 1400},
    {0xC13176, glue_C13176, "dispatch_event_sound", 650},
    {0xC12098, glue_C12098, "update_view_controls", 2100},
    {0xC1B27E, glue_C1B27E, "update_flight_input", 2400},
    {0xC3316E, glue_C3316E, "play_context_tone_4", 1200},
    {0xC244E2, glue_C244E2, "classify_selected_record_range", 1300},
    {0xC1342C, glue_C1342C, "update_matrix_side_record", 3500},
    {0xC2DD4E, glue_C2DD4E, "adjust_matrix_record_depth", 1300},
    /* render_line.c, render_state.c */
    {0xC2F490, glue_C2F490, "reset_line_style", 30},
    {0xC2F596, glue_C2F596, "clear_renderer_blocks", 500},
    {0xC1D722, glue_C1D722, "fill_column", 280},
    {0xC30F56, glue_C30F56, "start_blit", 90},
    /* audio.c, view.c, control_records.c */
    {0xC4FFB4, glue_C4FFB4, "clear_voice_interrupt", 60},
    {0xC08324, glue_C08324, "set_zoom_maximum", 60},
    {0xC095C0, glue_C095C0, "reset_player_record", 220},
    {0xC1C7F6, glue_C1C7F6, "classify_record_rate", 120},
    {0xC50212, glue_C50212, "step_voice_program", 200},
    {0xC4FFB0, glue_C4FFB0, "clear_voice_interrupt", 70},
    {0xC13B5A, glue_C13B5A, "update_record_5a", 120},
    {0xC14876, glue_C14876, "ease_record_26", 100},
    {0xC308E2, glue_C308E2, "restart_blit_cd", 70},
    {0xC30904, glue_C30904, "restart_blit_ad", 70},
    {0xC2DEA2, glue_C2DEA2, "mark_record_pending", 50},
    {0xC28F16, glue_C28F16, "set_record_view", 90},
    {0xC0FA4C, glue_C0FA4C, "await_viewport_match", 90},
    {0xC0FA80, glue_C0FA80, "complete_post_input", 80},
    {0xC1FE20, glue_C1FE20, "zero_result", 20},
    {0xC21960, glue_C21960, "zero_result", 24},
    {0xC25980, glue_C25980, "divide_rounded", 250},
    {0xC2E370, glue_C2E370, "y_rotation_matrix", 260},
    /* batch 8: depth sort, records, compass, cockpit, context stage */
    {0xC1E4A6, glue_C1E4A6, "sort_by_depth", 900},
    {0xC1E328, glue_C1E328, "sort_display_list", 2000},
    {0xC1CA82, glue_C1CA82, "flag_all_records", 300},
    {0xC1EC3A, glue_C1EC3A, "read_record_pair", 120},
    {0xC2DAF2, glue_C2DAF2, "update_view_matrix", 320},
    {0xC310AA, glue_C310AA, "update_compass", 300},
    {0xC082B8, glue_C082B8, "request_cockpit_redraw", 300},
    {0xC082B0, glue_C082B0, "finish_scene_setup", 320},
    {0xC10C08, glue_C10C08, "start_context_stage", 120},
    {0xC11B0E, glue_C11B0E, "clear_long_table", 500},
    /* batch 9: hex text, decay, nudge, random, voices, readout, mission, view pan */
    {0xC0F56A, glue_C0F56A, "format_hex", 600},
    {0xC13A2A, glue_C13A2A, "decay_toward_zero", 120},
    {0xC13CDE, glue_C13CDE, "nudge_outside_dead_zone", 120},
    {0xC13396, glue_C13396, "five_eighths", 80},
    {0xC50AB4, glue_C50AB4, "random_bit", 150},
    {0xC17B08, glue_C17B08, "free_voice", 150},
    {0xC2548A, glue_C2548A, "update_readout", 250},
    {0xC0840E, glue_C0840E, "reset_mission_objects", 900},
    {0xC258C8, glue_C258C8, "pan_view_from_keys", 120},
    /* batch 10: decay, messages, lookups, cell steps, 2.8 matrix, cached display value */
    {0xC148A2, glue_C148A2, "decay_outside_limit", 130},
    {0xC11312, glue_C11312, "reset_message_sequence", 150},
    {0xC287DA, glue_C287DA, "mode_offset", 80},
    {0xC1FEF2, glue_C1FEF2, "skip_stream_records", 60},
    {0xC1ECFC, glue_C1ECFC, "cell_step", 100},
    {0xC1ECD4, glue_C1ECD4, "cell_step", 100},
    {0xC2E346, glue_C2E346, "y_rotation_matrix8", 280},
    {0xC31C20, glue_C31C20, "display_value_to_draw", 80},
    /* batch 11: player setup, steering, random bits, channel stop, cockpit script */
    {0xC09620, glue_C09620, "prepare_player_record", 700},
    {0xC13BA0, glue_C13BA0, "steer_record_56", 220},
    {0xC13C64, glue_C13C64, "steer_record_5a", 240},
    {0xC50B02, glue_C50B02, "random_bits", 300},
    {0xC180FC, glue_C180FC, "stop_channel_2", 180},
    {0xC1EC96, glue_C1EC96, "cell_step", 130},
    {0xC21916, glue_C21916, "skip_for_type_3_to_6", 70},
    {0xC21966, glue_C21966, "skip_counted_entries", 90},
    {0xC2198C, glue_C2198C, "skip_for_low_class", 70},
    {0xC218C8, glue_C218C8, "skip_to_type_block", 110},
    {0xC207FE, glue_C207FE, "viewed_record_flagged", 70},
    /* batch 12: view octant, paired records, attitude term */
    {0xC254E8, glue_C254E8, "update_view_octant", 120},
    {0xC231A2, glue_C231A2, "paired_record_ready", 140},
    {0xC148E2, glue_C148E2, "attitude_term", 450},
    /* batch 13: decimal format, cockpit slide, position history */
    {0xC3267A, glue_C3267A, "format_decimal", 500},
    {0xC2559A, glue_C2559A, "step_cockpit_slide", 300},
    {0xC2651E, glue_C2651E, "record_position_history", 350},
    /* batch 14: paired sin_cos, print_number, square_root */
    {0xC2E5F6, glue_C2E5F6, "sin_cos", 200},
    {0xC24F76, glue_C24F76, "print_number", 700},
    {0xC2564E, glue_C2564E, "square_root", 600},
    /* batch 15: cell occupancy, record position, record 76/78 */
    {0xC1D520, glue_C1D520, "collect_records_in_cell", 600},
    {0xC1D0B6, glue_C1D0B6, "accumulate_record_position", 250},
    {0xC26428, glue_C26428, "update_record_76_78", 500},
    /* batch 16: small text */
    {0xC32794, glue_C32794, "draw_small_text", 400},
    {0xC32662, glue_C32662, "draw_small_text", 420},
    {0xC3271A, glue_C3271A, "format_small_hex", 600},
    {0xC32736, glue_C32736, "format_small_hex", 620},
    /* batch 17: BCD unpack, sorted search, record 56/66 with alert */
    {0xC259C2, glue_C259C2, "unpack_display_value", 900},
    {0xC1D4E4, glue_C1D4E4, "find_sorted_word", 200},
    {0xC13A8E, glue_C13A8E, "update_record_56_from_66", 200},
    /* batch 18: joystick */
    {0xC16F1C, glue_C16F1C, "read_joystick", 300},
    /* batch 19: rotation matrices and row scaling */
    {0xC2E47A, glue_C2E47A, "rotation_matrix", 900},
    {0xC2E38E, glue_C2E38E, "two_angle_matrix", 520},
    {0xC2E5AC, glue_C2E5AC, "scale_matrix_rows", 640},
    /* batch 19: three-angle matrix variants */
    {0xC2E3DE, glue_C2E3DE, "rotation_matrix8", 900},
    {0xC2E514, glue_C2E514, "alternate_rotation_matrix", 900},
    /* batch 20: inverse orientation, stream skip, attitude flags, vertex tail, divide, interrupt server */
    {0xC2D970, glue_C2D970, "inverse_orientation_matrix", 960},
    {0xC21940, glue_C21940, "skip_if_shown_record_flag", 90},
    {0xC122A2, glue_C122A2, "update_attitude_flags", 500},
    {0xC0D384, glue_C0D384, "derive_vertex_tail", 1300},
    {0xC52EC8, glue_C52EC8, "long_divide", 900},
    {0xC06132, glue_C06132, "count_interrupt", 60},
    /* batch 20: play_sound */
    {0xC17B2C, glue_C17B2C, "play_sound", 450},
    /* batch 21: side-plane clips, view transform */
    {0xC2EA5A, glue_C2EA5A, "clip_to_side_plane", 420},
    {0xC2EAD0, glue_C2EAD0, "clip_to_side_plane", 430},
    {0xC2F0C6, glue_C2F0C6, "clip_to_side_plane", 330},
    {0xC2F0F4, glue_C2F0F4, "clip_to_side_plane", 340},
    {0xC1F2EE, glue_C1F2EE, "view_transform", 560},
    /* batch 22: magnitude */
    {0xC1D974, glue_C1D974, "magnitude3", 500},
    /* batch 23: y-plane clips, sound routines, record orientation */
    {0xC2EB4C, glue_C2EB4C, "clip_to_view_plane", 430},
    {0xC2EBC2, glue_C2EBC2, "clip_to_view_plane", 440},
    {0xC2F156, glue_C2F156, "clip_to_view_plane", 340},
    {0xC17CF6, glue_C17CF6, "play_engine", 700},
    {0xC17DAA, glue_C17DAA, "slide_engine", 1500},
    {0xC17E4A, glue_C17E4A, "play_noise", 2000},
    {0xC17EF2, glue_C17EF2, "play_programmed_sound", 900},
    {0xC18096, glue_C18096, "play_scripted_sound", 700},
    {0xC2D954, glue_C2D954, "set_record_orientation", 2000},
    /* batch 24: local to world, shown vertices, normalize, slot scan */
    {0xC091E0, glue_C091E0, "local_to_world", 700},
    {0xC091CE, glue_C091CE, "local_to_world", 720},
    {0xC091A8, glue_C091A8, "local_to_world", 2600},
    {0xC0D334, glue_C0D334, "derive_shown_vertices", 2800},
    {0xC25754, glue_C25754, "normalize_vector", 1600},
    {0xC265E8, glue_C265E8, "flagged_slot_in_range", 3500},
    /* batch 25: main engine, tone, edge vertices, buffers, stage blit, grid position, list point */
    {0xC17C62, glue_C17C62, "play_main_engine", 900},
    {0xC17D6E, glue_C17D6E, "slide_main_engine", 1600},
    {0xC3316A, glue_C3316A, "play_tone", 1200},
    {0xC219AE, glue_C219AE, "derive_edge_vertices", 400},
    {0xC2FD22, glue_C2FD22, "clear_render_buffers", 150000},
    {0xC3040C, glue_C3040C, "blit_mask_between_planes", 300},
    {0xC1EBE0, glue_C1EBE0, "grid_relative_position", 300},
    {0xC25876, glue_C25876, "append_list_point", 600},
    /* batch 26: tones, page plane tops */
    {0xC33180, glue_C33180, "play_tone_2", 1200},
    {0xC3318E, glue_C3318E, "play_status_tone", 1250},
    {0xC33186, glue_C33186, "play_status_tone_outside_context", 1260},
    {0xC2F582, glue_C2F582, "clear_page_plane_tops", 600},
    /* batch 27: target point, record range, lane blit */
    {0xC1C2C8, glue_C1C2C8, "update_target_point", 1400},
    {0xC24568, glue_C24568, "classify_record_range", 1200},
    {0xC304FA, glue_C304FA, "blit_lane", 400},
    /* batch 28: post-input expiry, projection seed, condition tables */
    {0xC10D8A, glue_C10D8A, "check_post_input_expiry", 1300},
    {0xC1C54E, glue_C1C54E, "seed_projection", 1500},
    {0xC09AB8, glue_C09AB8, "condition_table_matches", 800},
    /* batch 29: component bound, repeated sum, view key */
    {0xC1FC42, glue_C1FC42, "component_beyond_bound", 250},
    {0xC1BA86, glue_C1BA86, "queue_view_key", 500},
    /* batch 30: audio interrupt, date line */
    {0xC50158, glue_C50158, "update_voices", 3000},
    {0xC24E2C, glue_C24E2C, "format_date_line", 1500},
    /* batch 31: condition flags, lost selection */
    {0xC09A78, glue_C09A78, "update_condition_a", 900},
    {0xC09A98, glue_C09A98, "update_condition_b", 900},
    {0xC12242, glue_C12242, "drop_lost_selection", 700},
    /* batch 32: fault hook, level lists */
    {0xC06C02, glue_C06C02, "fault_hook", 16},
    {0xC1D5D8, glue_C1D5D8, "file_records_by_level", 900},
    /* batch 33: pixel plots */
    {0xC2F5F4, glue_C2F5F4, "plot_pixel", 260},
    {0xC2F60A, glue_C2F60A, "plot_pixel_pair", 280},
    /* batch 34: polygon preparation */
    {0xC301F6, glue_C301F6, "prepare_polygon", 6000},
    /* batch 35: polygon submission */
    {0xC2FF48, glue_C2FF48, "draw_polygon", 9000},
    /* batch 36: clip stages */
    {0xC247C0, glue_C247C0, "clip_stage", 900},
    {0xC248B2, glue_C248B2, "clip_stage", 850},
    {0xC24996, glue_C24996, "clip_stage", 800},
    /* batch 37: outer polygon clipper */
    {0xC2469E, glue_C2469E, "clip_and_draw_polygon", 20000},
    {0xC246A0, glue_C246A0, "clip_and_draw_polygon", 20000},
    /* batch 38: faces and view marks */
    {0xC09952, glue_C09952, "draw_indexed_face", 30000},
    {0xC099F6, glue_C099F6, "draw_outlined_face", 30000},
    {0xC332FE, glue_C332FE, "draw_view_marker", 3000},
    {0xC30918, glue_C30918, "draw_gauge_bar", 15000},
    /* batch 39: cockpit messages */
    {0xC11BFC, glue_C11BFC, "update_message", 3000},
    /* batch 41: plane-side test */
    {0xC27456, glue_C27456, "faces_all_behind", 2500},
    /* batch 42: direction tracking */
    {0xC123FA, glue_C123FA, "track_direction", 6000},
    /* batch 43: normalize register entry */
    {0xC2574A, glue_C2574A, "normalize_vector", 1600},
    /* batch 44: view aiming */
    {0xC2D9BA, glue_C2D9BA, "aim_view", 12000},
    /* batch 45: face toward eye */
    {0xC1FB8C, glue_C1FB8C, "face_toward_eye", 900},
    /* batch 46: target distance */
    {0xC1D91A, glue_C1D91A, "target_distance", 700},
    /* batch 47: post-input stages, stick and throttle, flight recorder */
    {0xC0F946, glue_C0F946, "await_viewport_then_ready", 120},
    {0xC0F974, glue_C0F974, "mark_viewport_ready", 80},
    {0xC0FB70, glue_C0FB70, "choose_after_countdown", 250},
    {0xC0FBB6, glue_C0FBB6, "leave_on_key_or_message", 250},
    {0xC101FC, glue_C101FC, "reset_viewport_after_countdown", 100},
    {0xC10228, glue_C10228, "enter_mode_four_when_ready", 150},
    {0xC1072E, glue_C1072E, "queue_message_four", 100},
    {0xC1075A, glue_C1075A, "start_outcome_countdown", 250},
    {0xC11872, glue_C11872, "expire_to_fire_state", 120},
    {0xC118E6, glue_C118E6, "end_on_message", 60},
    {0xC11958, glue_C11958, "follow_message_or_phase", 150},
    {0xC119D4, glue_C119D4, "restart_after_countdown", 200},
    {0xC0A2F0, glue_C0A2F0, "begin_phase_three", 150},
    {0xC1B4D0, glue_C1B4D0, "set_throttle_input", 60},
    {0xC1B4D4, glue_C1B4D4, "set_throttle_input", 60},
    {0xC1B4D8, glue_C1B4D8, "release_throttle_keys", 70},
    {0xC1B4DE, glue_C1B4DE, "set_throttle_input", 60},
    {0xC1B50C, glue_C1B50C, "set_stick_y", 80},
    {0xC1B510, glue_C1B510, "set_stick_y", 80},
    {0xC1B514, glue_C1B514, "set_stick_y", 80},
    {0xC1B558, glue_C1B558, "set_stick_x", 80},
    {0xC1B55C, glue_C1B55C, "set_stick_x", 80},
    {0xC1B560, glue_C1B560, "set_stick_x", 80},
    {0xC25A6A, glue_C25A6A, "record_flight_input", 250},
    {0xC33DA4, glue_C33DA4, "take_warning_events", 80},
    {0xC13C0A, glue_C13C0A, "ease_record_58", 400},
    {0xC21C4C, glue_C21C4C, "split_edge", 300},
    {0xC1FED4, glue_C1FED4, "skip_word_for_mode_57", 50},
    {0xC345A0, glue_C345A0, "plot_ring", 20000},
    /* batch 48: coloured face, stored-normal test, record steering, view rotation, edge split */
    {0xC099AA, glue_C099AA, "draw_coloured_face", 30000},
    {0xC1FB9C, glue_C1FB9C, "point_toward_eye", 300},
    {0xC2CAA0, glue_C2CAA0, "steer_record_neutral", 60},
    {0xC2CA92, glue_C2CA92, "steer_record_roll", 80},
    {0xC2CA26, glue_C2CA26, "steer_record_turn", 200},
    {0xC2CB86, glue_C2CB86, "steer_record_pitch", 150},
    {0xC2CE82, glue_C2CE82, "rotate_by_view_matrix", 700},
    {0xC21C2E, glue_C21C2E, "split_record_and_stream_edges", 700},
    /* batch 49: projected segment, top-plane crossing, in-sight flag, edge alignment */
    {0xC2ED70, glue_C2ED70, "draw_projected_segment", 3000},
    {0xC2F128, glue_C2F128, "clip_to_view_plane", 340},
    {0xC2436A, glue_C2436A, "update_in_sight", 2500},
    {0xC2084A, glue_C2084A, "edge_alignment", 3000},
    {0xC2082A, glue_C2082A, "edge_alignment_test", 3000},
    /* batch 50: symbol plot */
    {0xC348B2, glue_C348B2, "plot_symbol", 4000},
    /* batch 51-52: clipped segment, ground points, voices, messages, observer, stages, long table, alert, start position, typed code */
    {0xC2EE4A, glue_C2EE4A, "draw_clipped_segment", 5000},
    {0xC098C6, glue_C098C6, "transform_ground_points", 3000},
    {0xC0F4A6, glue_C0F4A6, "free_all_voices", 600},
    {0xC25704, glue_C25704, "post_message", 200},
    {0xC0915A, glue_C0915A, "set_observer_position", 150},
    {0xC11078, glue_C11078, "raise_event_after_countdown", 120},
    {0xC11ACC, glue_C11ACC, "load_long_table", 800},
    {0xC1803C, glue_C1803C, "sound_chosen_record_alert", 500},
    {0xC10678, glue_C10678, "queue_mode_messages", 300},
    {0xC0910C, glue_C0910C, "start_position", 40},
    {0xC25246, glue_C25246, "check_typed_code", 300},
    /* batch 53: draw-stream commands */
    {0xC212B0, glue_C212B0, "draw_segment_pairs", 20000},
    {0xC2129C, glue_C2129C, "draw_segment_pairs_near", 20000},
    {0xC211DC, glue_C211DC, "draw_segment_run", 20000},
    {0xC2131C, glue_C2131C, "draw_offset_segments", 20000},
    {0xC20E4E, glue_C20E4E, "draw_parallelogram_face", 30000},
    {0xC20E40, glue_C20E40, "draw_parallelogram_face_2", 30000},
    {0xC21490, glue_C21490, "draw_parallelogram_face_near", 30000},
    {0xC2139E, glue_C2139E, "draw_offset_face", 30000},
    {0xC21412, glue_C21412, "draw_mixed_face", 30000},
    {0xC20F10, glue_C20F10, "extend_parallelograms", 400},
    {0xC20EC4, glue_C20EC4, "extend_parallelograms_scaled", 600},
    /* batch 54: segment grids, block generators */
    {0xC20D68, glue_C20D68, "draw_segment_grid", 40000},
    {0xC20904, glue_C20904, "draw_segment_lattice", 40000},
    {0xC21A20, glue_C21A20, "offset_block_copies", 1500},
    {0xC217EA, glue_C217EA, "extend_block_scaled", 1500},
    /* batch 55: corner edges */
    {0xC2E758, glue_C2E758, "project_corner_edges", 20000},
    /* batch 56: fixed-row line, text lines and digits */
    {0xC2FA78, glue_C2FA78, "draw_line_to_row", 700},
    {0xC32726, glue_C32726, "format_digits", 620},
    {0xC32AB4, glue_C32AB4, "draw_text_in_view", 4000},
    {0xC32AA6, glue_C32AA6, "print_bcd_in_view", 4500},
    {0xC32AA4, glue_C32AA4, "print_bcd_in_view", 4500},
    /* batch 57-58: side face, quad list and strip, face grids and lattices */
    {0xC2159E, glue_C2159E, "draw_side_face", 30000},
    {0xC210E6, glue_C210E6, "draw_quad_strip", 60000},
    /* batch 59: cockpit readouts */
    {0xC2F5C0, glue_C2F5C0, "plot_pixel_in_view", 280},
    {0xC2F5D4, glue_C2F5D4, "plot_pixel", 270},
    {0xC31A64, glue_C31A64, "draw_scale_readout", 900},
    {0xC31ACC, glue_C31ACC, "draw_zoom_readout", 1100},
    {0xC31F4C, glue_C31F4C, "draw_speed_readout", 1000},
    {0xC3201A, glue_C3201A, "draw_altitude_readout", 1000},
    {0xC3212A, glue_C3212A, "draw_record_72_readout", 900},
    {0xC32178, glue_C32178, "draw_record_2b_readout", 900},
    {0xC321D2, glue_C321D2, "draw_grid_z_readout", 1500},
    {0xC32260, glue_C32260, "draw_grid_x_readout", 1500},
    {0xC31EB6, glue_C31EB6, "draw_heading_readout", 1500},
    {0xC31C60, glue_C31C60, "draw_weapon_readout", 4800},
    {0xC31D16, glue_C31D16, "draw_signed_readout", 4800},
    {0xC31E6C, glue_C31E6C, "draw_shoot_cue", 4200},
    {0xC31D64, glue_C31D64, "draw_load_readout", 5200},
    {0xC33F54, glue_C33F54, "draw_three_digits", 4600},
    /* batch 59b: weapon status, threat lights */
    {0xC2F64E, glue_C2F64E, "plot_square", 280},
    {0xC2F63A, glue_C2F63A, "plot_square_in_view", 290},
    {0xC328A8, glue_C328A8, "draw_weapon_status", 1500},
    {0xC3112A, glue_C3112A, "draw_threat_lights", 3000},
    /* batch 60: cockpit bars and panel image */
    {0xC30CC4, glue_C30CC4, "fill_bar_words", 900},
    {0xC30B5C, glue_C30B5C, "draw_indicator_bars", 2500},
    {0xC30D34, glue_C30D34, "draw_mode_bar", 2500},
    {0xC30EAA, glue_C30EAA, "blit_image", 3000},
    {0xC309B6, glue_C309B6, "draw_panel_image", 3000},
    /* batch 60b: compass tape */
    {0xC30F78, glue_C30F78, "draw_compass_tape", 3000},
    /* batch 60c: panel frame */
    {0xC30764, glue_C30764, "draw_panel_frame", 4000},
    /* batch 61: HUD marks */
    {0xC34146, glue_C34146, "draw_hud_marks", 6000},
    {0xC34066, glue_C34066, "draw_tick_row", 3000},
    /* batch 61b: target box */
    {0xC342D0, glue_C342D0, "draw_target_box", 6000},
    /* batch 61d: ring point, pixel block */
    {0xC2F66E, glue_C2F66E, "plot_pixel_block", 300},
    {0xC347F2, glue_C347F2, "plot_ring_point", 2500},
    /* batch 61e: missile cue */
    {0xC33DC8, glue_C33DC8, "update_missile_cue", 5000},
    /* batch 62: message line, display list sort */
    {0xC322EE, glue_C322EE, "draw_message_line", 9000},
    /* batch 63: postflight HUD */
    {0xC33CD2, glue_C33CD2, "transform_postflight_record", 1500},
    /* batch 63b */
    {0xC33B38, glue_C33B38, "draw_postflight_variant", 20000},
    /* batch 63c */
    {0xC33370, glue_C33370, "draw_postflight_tape", 40000},
    /* batch 63d: HUD stage */
    {0xC332BC, glue_C332BC, "draw_postflight_hud", 90000},
    /* polygon to row C7 */
    {0xC301F0, glue_C301F0, "prepare_polygon_to_row", 6000},
    /* zone exit */
    {0xC28E28, glue_C28E28, "check_zone_exit", 1500},
    /* shape */
    {0xC2D16C, glue_C2D16C, "draw_shape", 20000},
    /* block face */
    {0xC21500, glue_C21500, "draw_block_face", 8000},
    /* offset run */
    {0xC2122A, glue_C2122A, "draw_offset_run", 6000},
    /* split square */
    {0xC20592, glue_C20592, "draw_split_square", 8000},
    /* side triangle */
    {0xC2168A, glue_C2168A, "draw_side_triangle", 8000},
    /* square faces */
    {0xC203D0, glue_C203D0, "draw_square_faces", 16000},
    /* shadow */
    {0xC201A6, glue_C201A6, "draw_record_shadow", 30000},
    /* mark polygon */
    {0xC3019C, glue_C3019C, "draw_mark_polygon", 9000},
    /* face test dispatch */
    {0xC1FB82, glue_C1FB82, "face_toward_eye", 900},
    /* bound points */
    {0xC1F99A, glue_C1F99A, "transform_bound_points", 9000},
    /* draw_stream.c */
    {0xC1FF0A, glue_C1FF0A, "test_stream_face", 240},
    /* draw_stream.c */
    {0xC2005C, glue_C2005C, "draw_tested_face", 520},
    {0xC20100, glue_C20100, "draw_indexed_face_list", 620},
    /* draw_stream.c face loops */
    {0xC21060, glue_C21060, "draw_quad_list", 900},
    {0xC20C38, glue_C20C38, "draw_face_grid", 1200},
    {0xC20C22, glue_C20C22, "draw_face_grid_plain", 1200},
    /* draw_stream.c */
    {0xC20002, glue_C20002, "draw_tested_parallelogram", 420},
    /* control_records.c */
    {0xC1D3F4, glue_C1D3F4, "expand_cell_templates", 900},
    /* hud_bars.c */
    /* control_records.c */
    {0xC28800, glue_C28800, "aim_record_at_view", 9000},
    /* scene_setup.c */
    {0xC0924A, glue_C0924A, "reset_scene_context", 3200},
    {0xC09266, glue_C09266, "reset_scene_recorder", 3100},
    {0xC092A0, glue_C092A0, "place_scene_root", 3000},
    /* postflight scene callbacks (/) */
    {0xC11788, glue_C11788, "advance_postflight_reset", 3200},
    {0xC11830, glue_C11830, "restart_postflight_scene", 3900},
    /* panel mark drawing () */
    {0xC3003A, glue_C3003A, "draw_panel_mark", 12000},
    /* post-input heading formatter () */
    {0xC25070, glue_C25070, "refresh_post_input_heading", 5500},
    /* scene record stream dispatch () */
    {0xC28B34, glue_C28B34, "dispatch_scene_records", 12000},
    /* scene record initialization and aim () */
    {0xC28AFE, glue_C28AFE, "initialize_scene_record", 18000},
    /* post-input context command and heading marker () */
    {0xC10C68, glue_C10C68, "queue_post_input_context_command", 3000},
    /* static template bit-gate builder () */
    {0xC1C40C, glue_C1C40C, "build_template_bit_gates", 20000},
    /* selected-fire record initializer () */
    {0xC2374C, glue_C2374C, "consume_selected_fire_request", 9000},
    /* scene stream selection and special scene record () */
    {0xC28722, glue_C28722, "initialize_scene_from_mode", 24000},
    /* scene initialization and ordered child calls () */
    {0xC0FAA4, glue_C0FAA4, "initialize_scene_state", 32000},
    {0, 0, 0, 0}, /* sentinel; entries are added above it */
};
const int fa18_port_count = (int)(sizeof fa18_ports / sizeof fa18_ports[0]) - 1;
