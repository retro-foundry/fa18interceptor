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
    {0, 0, 0, 0}, /* sentinel; entries are added above it */
};
const int fa18_port_count = (int)(sizeof fa18_ports / sizeof fa18_ports[0]) - 1;
