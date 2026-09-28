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
    {0, 0, 0, 0}, /* sentinel; entries are added above it */
};
const int fa18_port_count = (int)(sizeof fa18_ports / sizeof fa18_ports[0]) - 1;
