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
    {0, 0, 0, 0}, /* sentinel; entries are added above it */
};
const int fa18_port_count = (int)(sizeof fa18_ports / sizeof fa18_ports[0]) - 1;
