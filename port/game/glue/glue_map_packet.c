/* Register bridge for the normal and wide map packet passes. */
#include "glue.h"
#include "ports_glue.h"

#include "map_packet.h"
#include "glue_clip.h"
#include "globals.h"
#include "polygon_clip.h"

#include <stdio.h>
#include <stdlib.h>

static int draw_map_polygon(void *context, MapPacketRegisterEffects *effects) {
    ClipperSnapshot snapshot;
    uint16_t colour = rd_u16(CURRENT_COLOUR);
    unsigned i;
    int drawn;
    (void)context;
    for (i = 0; i < 6; ++i) A(i) = effects->address[i];
    for (i = 0; i < 8; ++i) D(i) = effects->data[i];
    clipper_snapshot(&snapshot);
    drawn = clip_and_draw_polygon();
    clipper_registers(&snapshot, colour, drawn);
    for (i = 0; i < 6; ++i) effects->address[i] = A(i);
    for (i = 0; i < 8; ++i) effects->data[i] = D(i);
    return 0;
}

static int map_packet_glue(int wide) {
    MapPacketRegisterEffects effects;
    unsigned i;
    for (i = 0; i < 6; ++i) effects.address[i] = A(i);
    for (i = 0; i < 8; ++i) effects.data[i] = D(i);
    effects.draw_polygon = draw_map_polygon;
    effects.draw_context = 0;
    if (run_map_packet_pass(A(6), wide, &effects) != 0) {
        fprintf(stderr, "map packet %s pass failed at A6=%08x\n",
                wide ? "wide" : "normal", (unsigned)A(6));
        abort();
    }
    for (i = 0; i < 6; ++i) A(i) = effects.address[i];
    for (i = 0; i < 8; ++i) D(i) = effects.data[i];
    return glue_return();
}

int glue_C2AB34(void) { return map_packet_glue(1); }
int glue_C2AB5A(void) { return map_packet_glue(0); }
