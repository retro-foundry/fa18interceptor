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
    {0, 0, 0, 0}, /* sentinel; entries are added above it */
};
const int fa18_port_count = (int)(sizeof fa18_ports / sizeof fa18_ports[0]) - 1;
