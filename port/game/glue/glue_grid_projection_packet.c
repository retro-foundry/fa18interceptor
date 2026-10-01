/* CPU effects of the complete grid projection packet ($C279D0). */
#include "glue.h"
#include "ports_glue.h"
#include "grid_projection_packet.h"
#include "globals.h"

static void grid_gate(void *context, uint8_t mode, int32_t threshold, int32_t depth) {
    (void)context;
    SET_W(D(0), 4); SET_W(D(3), 0xffff); SET_B(D(3), mode);
    if (mode) SET_B(D(3), (uint8_t)(mode - 1));
    D(2) = (uint32_t)threshold; D(1) = (uint32_t)depth;
}
static void grid_setup(void *context, const GridProjectionSetup *setup) {
    (void)context;
    A(3) = setup->records; A(0) = VIEW_ANGLE_MATRIX + 2;
    D(0) = setup->records;
    if (setup->shift) SET_W(D(0), setup->scaled_input);
    D(1) = setup->base_products[0]; D(2) = setup->base_products[1];
    D(3) = setup->base_products[2]; SET_W(D(4), 0xff20);
}
static void grid_record(void *context, const GridProjectionSetup *setup,
                         const GridProjectionRecord *record) {
    (void)context;
    A(3) = record->next_record; A(4) = 0xc27d24u;
    D(0) = (uint32_t)(int32_t)record->source_x; SET_W(D(0), record->x);
    D(1) = (uint32_t)(int32_t)record->source_y; SET_W(D(1), record->y);
    D(2) = (uint32_t)(int32_t)record->source_kind;
    SET_W(D(3), record->bound); SET_W(D(4), record->bounds_index);
    if (!record->admitted) return;
    SET_W(D(2), record->kind); SET_W(D(3), setup->shift);
    SET_W(D(0), record->shifted_x); SET_W(D(1), record->shifted_y);
    A(4) = (uint32_t)(int32_t)record->shifted_x;
    A(5) = (uint32_t)(int32_t)record->shifted_y;
    D(1) = (uint32_t)(int32_t)setup->base[0];
    D(5) = (uint32_t)(int32_t)setup->base[1];
    D(7) = (uint32_t)(int32_t)setup->base[2];
}
static void grid_point(void *context, const GridProjectionSetup *setup,
                       const GridProjectionRecord *record, const GridProjectionPoint *point) {
    (void)context; (void)setup; (void)record;
    A(0) = VIEW_ANGLE_MATRIX + 14;
    D(0) = point->vertical_first_product; D(2) = point->vertical;
    D(3) = point->depth_first_product; D(4) = point->depth;
    SET_W(D(6), point->horizontal_component);
    if (point->comparisons >= 2) SET_W(D(0), (uint16_t)(0u - (uint16_t)point->horizontal_component));
    if (point->comparisons >= 4) SET_W(D(0), (uint16_t)(0u - (uint16_t)point->vertical));
    if (point->accepted) { D(6) = point->screen_x; D(2) = point->screen_y; }
    if (point->triangle) {
        A(1) = point->output + (point->accepted ? 4u : 0u);
        A(2) = point->next_pair;
    }
}
static void grid_draw_triangle(void *context) {
    uint32_t cursor = A(3);
    (void)context;
    m68ki_push_32(cursor); m68ki_push_32(0xc27c3au);
    glue_C2FF48(); A(3) = m68ki_pull_32();
}
static void grid_draw_pixel(void *context, int16_t x, int16_t y, int adjacent) {
    uint32_t cursor = A(3);
    (void)context;
    SET_W(D(0), x); SET_W(D(1), y);
    m68ki_push_32(cursor); m68ki_push_32(adjacent ? 0xc27d0au : 0xc27d02u);
    if (adjacent) glue_C2F60A(); else glue_C2F5F4();
    A(3) = m68ki_pull_32();
}
int glue_C279D0(void) {
    const GridProjectionHooks hooks = {
        0, grid_gate, grid_setup, grid_record, grid_point, grid_draw_triangle, grid_draw_pixel
    };
    m68ki_push_32(A(6)); A(6) = A(7); A(7) -= 0x24;
    draw_grid_projection_packet(A(6), &hooks);
    A(7) = A(6); A(6) = m68ki_pull_32();
    return glue_return();
}
