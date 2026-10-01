/* Register bridge for the normal and wide map packet passes. */
#include "glue.h"
#include "ports_glue.h"

#include "map_packet.h"
#include "glue_clip.h"
#include "globals.h"
#include "polygon_clip.h"
#include "../../map_packet_static_data.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    FA18MapPacketPassSelectorResult pass;
    gaddr component;
    uint32_t control_cursor;
} MapPacketBridge;

static int draw_map_polygon(void *context, gaddr projected_end, int16_t origin_x) {
    ClipperSnapshot snapshot;
    uint16_t colour = rd_u16(CURRENT_COLOUR);
    uint32_t saved_d0 = D(0);
    int drawn;
    (void)context;
    A(5) = projected_end;
    /* $C2AF8C MOVEM.W sign-extends the origin into D6. */
    D(6) = (uint32_t)(int32_t)origin_x;
    clipper_snapshot(&snapshot);
    drawn = clip_and_draw_polygon();
    clipper_registers(&snapshot, colour, drawn);
    D(0) = saved_d0; /* $C2AFE8 restores D0 after the child. */
    return 0;
}

static void begin_map_pass(void *context, gaddr component,
                           const FA18MapPacketPassSelectorResult *pass) {
    MapPacketBridge *bridge = context;
    bridge->pass = *pass;
    bridge->component = component;
    A(4) = component;
    A(0) = pass->control_stream_address;
}

static void control_registers(void *context, uint8_t mode, const int8_t pair[2]) {
    MapPacketBridge *bridge = context;
    const FA18MapPacketPassSelectorResult *pass = &bridge->pass;
    int16_t row = (int16_t)(uint16_t)((uint16_t)pass->coordinate.row_min +
                                     (uint16_t)(int16_t)pair[0]);
    int16_t column = (int16_t)(uint16_t)((uint16_t)pass->coordinate.column_min +
                                        (uint16_t)(int16_t)pair[1]);
    uint16_t shifted_column, index;

    A(0) = pass->control_stream_address + ++bridge->control_cursor;
    A(2) = FA18_MAP_PACKET_CONTROL_RUNTIME_BASE + 2u * mode;
    SET_W(D(2), 2u * mode);
    SET_W(D(3), (uint16_t)(int16_t)pair[0]);
    SET_W(D(0), row);
    if (row < 0 || row > pass->directory.row_max) return;
    SET_W(D(3), (uint16_t)(int16_t)pair[1]);
    SET_W(D(1), column);
    if (column < 0 || column > pass->directory.column_max) return;
    shifted_column = (uint16_t)((uint16_t)column <<
        (pass->directory.layout == FA18_MAP_PACKET_DIRECTORY_WIDE ? 6u : 4u));
    index = (uint16_t)((uint16_t)row * 2u + shifted_column);
    SET_W(D(0), index);
    SET_W(D(1), shifted_column);
    SET_W(D(0), rd_u16(pass->directory.record_base_address + index));
    A(1) = A(3) = pass->directory.record_base_address;
}

static void cursor_registers(void *context, gaddr next_word, int16_t last_word,
                              uint8_t detail_cutoff) {
    (void)context;
    A(3) = next_word;
    if (detail_cutoff) {
        uint16_t relative = (uint16_t)(((uint16_t)last_word & 0x7fffu) << 2);
        D(1) = (uint32_t)(int32_t)(int16_t)relative;
    } else SET_W(D(1), last_word);
}

static void packet_registers(void *context, gaddr packet) {
    (void)context;
    A(3) = packet;
}

static void seed_registers(void *context, uint32_t packed_seed) {
    (void)context;
    D(0) = packed_seed;
}

static void transform_registers(void *context, uint32_t last_y) {
    (void)context;
    D(7) = last_y;
}

static void visibility_registers(void *context, uint32_t limit) {
    (void)context;
    D(6) = limit;
}

static void detail_registers(void *context, uint8_t detail, uint16_t visibility) {
    (void)context;
    if (!detail && visibility) D(6) = 0; /* $C2AE76 */
}

static void end_map_pass(void *context, gaddr next_control, int terminated) {
    MapPacketBridge *bridge = context;
    A(0) = next_control;
    if (terminated) SET_W(D(2), 0xffffu);
    A(4) = bridge->component;
}

static void map_packet_registers(int wide) {
    MapPacketBridge bridge = {0};
    const MapPacketHooks hooks = {
        &bridge, draw_map_polygon, begin_map_pass, control_registers,
        cursor_registers, packet_registers, seed_registers,
        transform_registers, visibility_registers, detail_registers, end_map_pass
    };
    if (run_map_packet_pass(A(6), wide, &hooks) != 0) {
        fprintf(stderr, "map packet %s pass failed at A6=%08x\n",
                wide ? "wide" : "normal", (unsigned)A(6));
        abort();
    }
}

int glue_C2AB34(void) { map_packet_registers(1); return glue_return(); }
int glue_C2AB5A(void) { map_packet_registers(0); return glue_return(); }

int glue_C2AA9C(void) {
    uint32_t old_frame = A(6), depth = 0u - rd_u32(PROJECTION_Y);
    FA18MapPacketDepthStageResult result;
    A(7) -= 4; wr_u32(A(7), old_frame); A(6) = A(7); A(7) -= 0x46;
    result = prepare_map_packet_depth(A(6));
    SET_W(D(0), result.renderer_words[0]); SET_W(D(1), result.renderer_words[1]);
    SET_W(D(2), 0); SET_W(D(3), 0);
    D(0) = (uint32_t)result.metric;
    if (!rd_u8(ZOOM_FLAGS) && (int32_t)depth <= 0x7fff0) {
        uint16_t scale = rd_u16(ZOOM_SCALE);
        uint16_t divisor = (int16_t)scale < 2 ? 2u : scale;
        D(7) = ((0x8000u % divisor) << 16) | (0x8000u / divisor);
    }
    if (result.run_normal_pass) {
        wr_u16(CURRENT_COLOUR, 6); map_packet_registers(0);
    }
    wr_u16(CURRENT_COLOUR, 6); map_packet_registers(1);
    A(7) = A(6); A(6) = rd_u32(A(7)); A(7) += 4;
    return glue_return();
}
