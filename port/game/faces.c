/* Faces drawn from the transformed vertex table. */
#include "faces.h"

#include "globals.h"
#include "polygon_clip.h"

/* Copy the vertex at `v` to `*dst`; returns its z. */
static uint16_t copy_vertex(gaddr *dst, gaddr v) {
    wr_u32(*dst, rd_u32(v));
    wr_u16(*dst + 4, rd_u16(v + 4));
    *dst += 6;
    return rd_u16(v + 4);
}

static gaddr start_input(int16_t count) {
    wr_u16(CLIP_INPUT, 0); /* no shift */
    wr_s16(CLIP_INPUT + 2, count);
    return CLIP_INPUT + 4;
}

int draw_indexed_face(gaddr *face) {
    int16_t count = rd_s16(*face), i, reads = count < 3 ? 3 : count;
    gaddr dst = start_input(count);
    uint16_t behind = 0xFFFF;

    *face += 2;
    for (i = 0; i < reads; i++, *face += 2) behind &= copy_vertex(&dst, WORKSPACES + (gaddr)(int32_t)rd_s16(*face));
    wr_u16(CURRENT_COLOUR, rd_u16(*face));
    *face += 2;
    if ((int16_t)behind < 0) return 0;
    return clip_and_draw_polygon();
}

int draw_outlined_face(gaddr *face) {
    int16_t count = rd_s16(*face);
    gaddr v = WORKSPACES + (gaddr)(int32_t)rd_s16(*face + 2), dst = start_input(count);
    uint16_t behind = 0xFFFF;
    uint32_t i, copies = (uint32_t)(uint16_t)(count - 3) + 4; /* count + 1 */
    int drawn;

    *face += 4;
    for (i = 0; i < copies; i++, v += 6) behind &= copy_vertex(&dst, v);
    if ((int16_t)behind < 0) return 0;
    wr_u16(CURRENT_COLOUR, 7);
    wr_u16(LINE_STYLE, 3);      /* planes */
    wr_u16(LINE_STYLE + 2, 3);  /* colour */
    drawn = clip_and_draw_polygon();
    wr_u16(LINE_STYLE, 0xF);
    wr_u16(LINE_STYLE + 2, 0xFFFF);
    return drawn;
}

void split_edge(gaddr points) {
    int16_t p[3], d[3];
    int k;
    for (k = 0; k < 3; k++) {
        p[k] = rd_s16(points + (gaddr)(2 * k));
        d[k] = (int16_t)((int16_t)(rd_s16(points + 6 + (gaddr)(2 * k)) - p[k]) >> 1);
        wr_s16(points + 0x1E + (gaddr)(2 * k), (int16_t)(p[k] + d[k]));
    }
    for (k = 0; k < 3; k++)
        wr_s16(points + 0x24 + (gaddr)(2 * k), (int16_t)(p[k] + (int16_t)(d[k] >> 1)));
}
