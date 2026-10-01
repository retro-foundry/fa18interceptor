/* Register replay for $C2B05A. The shape walk overwrites D0-D5 and A0/A3;
 * only retained directory results need replay before that walk. */
#include "glue.h"
#include "ports_glue.h"
#include "record_region_probe.h"
#include "globals.h"

static uint32_t swap(uint32_t value) {
    return (value << 16) | (value >> 16);
}

static void directory_registers(void *context, gaddr record, uint32_t x,
                                uint32_t y, uint16_t index, int16_t offset) {
    (void)context;
    A(1) = record;
    D(6) = swap(x);
    SET_W(D(6), index);
    D(7) = swap(y);
    SET_W(D(7), offset);
}

static void remaining_registers(void *context, int16_t count) {
    (void)context;
    SET_W(D(7), count);
}

static void group_registers(void *context, gaddr first_vertex, uint16_t crossings) {
    (void)context;
    A(4) = first_vertex;
    A(5) = crossings;
}

static void segment_registers(void *context, const RegionSegmentProbe *probe) {
    (void)context;
    if (probe->stage == REGION_SEGMENT_NORMALIZED) D(6) = probe->magnitude;
    else {
        A(2) = probe->delta_y;
        if (probe->stage == REGION_SEGMENT_PROJECTED)
            D(6) = 0xffff0000u | (uint16_t)probe->scale;
        else {
            /* $C2B1CA restores the saved D2-D5 into D2/D4-D6, so D6
             * receives endpoint Y before the bound-order exchange. */
            D(6) = probe->upper_y;
        }
    }
}

static void shape_registers(void *context, uint16_t offset, const int16_t fields[5]) {
    unsigned i;
    (void)context;
    A(0) = 0xc42a96u;
    A(3) = SCENE_POINTERS;
    D(0) = offset;
    for (i = 0; i < 5; ++i) D(i + 1) = (uint32_t)(int32_t)fields[i];
}

static void position_registers(void *context, const RegionShapePosition *position) {
    (void)context;
    D(1) = position->origin[0];
    D(2) = position->origin[1];
    SET_W(D(3), position->adjustment_index);
    D(4) = position->offset[0];
    D(5) = position->offset[1];
    D(6) = position->adjustment[0];
    D(7) = position->adjustment[1];
    A(4) = GRID_ADJUST_WORDS;
}

static void distance_registers(void *context, uint32_t magnitude) {
    (void)context;
    D(3) = magnitude;
}

static void stream_registers(void *context, int16_t selector, gaddr stream) {
    (void)context;
    SET_W(D(5), selector);
    D(6) = stream;
}

static void polygon_registers(void *context, gaddr polygon) {
    (void)context;
    A(4) = polygon;
}

static void edge_registers(void *context, const RegionShapeEdge *edge) {
    (void)context;
    D(3) = edge->relative_x;
    D(4) = edge->relative_y;
    D(5) = edge->cross_product;
    D(6) = edge->x_product;
}

int glue_C2B05A(void) {
    const RecordRegionProbeHooks hooks = {
        0, directory_registers, remaining_registers, group_registers,
        segment_registers, shape_registers, position_registers,
        distance_registers, stream_registers, polygon_registers, edge_registers
    };
    probe_record_regions(&hooks);
    return glue_return();
}
