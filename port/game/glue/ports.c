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
    {0, 0, 0, 0}, /* sentinel; entries are added above it */
};
const int fa18_port_count = (int)(sizeof fa18_ports / sizeof fa18_ports[0]) - 1;
