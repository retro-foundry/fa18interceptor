#include "projection_grid.h"
#include "line.h"

#include <assert.h>
#include <string.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

typedef struct {
    FA18IndexedFrameBuffer *framebuffer;
    const FA18LineStyle *style;
} C301F6LineAdapter;

typedef struct {
    int calls;
    uint16_t value;
} C2FF48DmaAdapter;

typedef struct {
    int calls;
    int16_t x;
    int16_t y;
} C302C4AxisAdapter;

typedef struct {
    int primary_calls;
    int adjacent_calls;
} C27GridTraversalAdapter;

typedef struct {
    int calls;
    FA18ProjectionPairBounds calls_seen[4];
} C302E6RangeAdapter;

typedef struct {
    int calls;
    FA18ProjectionPairBounds segment;
    uint32_t scratch;
} C3029ELineAdapter;

typedef struct {
    int blitter_calls;
    int final_calls;
    FA18ProjectionPairBlitterWrites first_blitter;
    FA18ProjectionPairBlitterWrites blitter[4];
    FA18ProjectionPairFinalState final_state;
} C301FarListAdapter;

static int emit_c301f6_blitter_line(void *context,
                                    int16_t x0, int16_t y0,
                                    int16_t x1, int16_t y1,
                                    int16_t display_bound_y) {
    C301F6LineAdapter *adapter = context;
    if (!adapter) return -1;
    return fa18_draw_line(adapter->framebuffer, adapter->style,
                          (FA18LineSegment){x0, y0, x1, y1},
                          display_bound_y) < 0 ? -1 : 0;
}

static int emit_c2ff48_dma(void *context, uint16_t value) {
    C2FF48DmaAdapter *adapter = context;
    if (!adapter) return -1;
    ++adapter->calls;
    adapter->value = value;
    return 0;
}

static int emit_c302c4_axis(void *context, int16_t x, int16_t y) {
    C302C4AxisAdapter *adapter = context;
    if (!adapter) return -1;
    ++adapter->calls;
    adapter->x = x;
    adapter->y = y;
    return 0;
}

static int emit_c27_primary_axis(void *context, int16_t x, int16_t y) {
    C27GridTraversalAdapter *adapter = context;
    if (!adapter || x < 0 || y < 0) return -1;
    ++adapter->primary_calls;
    return 0;
}

static int emit_c27_adjacent_axis(void *context, int16_t x, int16_t y) {
    C27GridTraversalAdapter *adapter = context;
    if (!adapter || x < 0 || y < 0) return -1;
    ++adapter->adjacent_calls;
    return 0;
}

static int emit_c302e6_range(void *context,
                              int16_t d0, int16_t d1,
                              int16_t d2, int16_t d3,
                              int16_t display_bound_y) {
    C302E6RangeAdapter *adapter = context;
    if (!adapter || adapter->calls >= 4 || display_bound_y != 144) return -1;
    adapter->calls_seen[adapter->calls++] = (FA18ProjectionPairBounds){d0, d2, d1, d3};
    return 0;
}

static int emit_c3029e_line(void *context,
                             int16_t x0, int16_t y0,
                             int16_t x1, int16_t y1,
                             uint32_t scratch) {
    C3029ELineAdapter *adapter = context;
    if (!adapter) return -1;
    ++adapter->calls;
    adapter->segment = (FA18ProjectionPairBounds){x0, x1, y0, y1};
    adapter->scratch = scratch;
    return 0;
}

static int emit_c301_far_blitter(void *context,
                                 const FA18ProjectionPairBlitterWrites *writes) {
    C301FarListAdapter *adapter = context;
    if (!adapter || !writes) return -1;
    if (!adapter->blitter_calls) adapter->first_blitter = *writes;
    if (adapter->blitter_calls < 4) adapter->blitter[adapter->blitter_calls] = *writes;
    ++adapter->blitter_calls;
    return 0;
}

static int emit_c301_far_final(void *context,
                               const FA18ProjectionPairFinalState *state) {
    C301FarListAdapter *adapter = context;
    if (!adapter || !state) return -1;
    ++adapter->final_calls;
    adapter->final_state = *state;
    return 0;
}

int main(void) {
#ifdef _MSC_VER
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
    uint8_t data[FA18_C279_PROJECTION_PAIR_SOURCE_20_OFFSET +
                 FA18_C279_PROJECTION_PAIR_SOURCE_BYTES] = { 0 };
    FA18HunkSegment segments[26];
    memset(segments, 0, sizeof segments);
    segments[FA18_C279_PROJECTION_GRID_HUNK].data = data;
    segments[FA18_C279_PROJECTION_GRID_HUNK].size = sizeof data;
    FA18Hunks hunks = { segments, 26 };
    FA18ProjectionGrid grid;
    FA18ProjectionGridRecord record;

    uint8_t *table = data + FA18_C279_PROJECTION_GRID_OFFSET;
    table[0] = 0; table[1] = 2;
    table[2] = 0; table[3] = 3;
    table[4] = 0x05; table[5] = 0x40;
    table[6] = 0xf9; table[7] = 0x80;
    table[8] = 0xff; table[9] = 0xec;
    table[10] = 0x01; table[11] = 0x60;
    table[12] = 0x05; table[13] = 0x80;
    table[14] = 0xff; table[15] = 0xf0;
    uint8_t *pairs_20 = data + FA18_C279_PROJECTION_PAIR_SOURCE_20_OFFSET;
    pairs_20[0] = 0; pairs_20[1] = 43;
    pairs_20[2] = 0xff; pairs_20[3] = 0xd5;
    pairs_20[4] = 0xff; pairs_20[5] = 0xd5;
    pairs_20[6] = 0xff; pairs_20[7] = 0xd5;
    pairs_20[8] = 0; pairs_20[9] = 0;
    pairs_20[10] = 0; pairs_20[11] = 32;

    assert(fa18_load_projection_grid(&hunks, &grid) == 0);
    assert(grid.record_count == 2);
    assert(grid.bounds_limit == 3);
    assert(fa18_projection_grid_record(&grid, 0, &record) == 0);
    assert(record.x == 0x0540 && record.y == -0x0680 && record.kind == -20);
    assert(fa18_projection_grid_record(&grid, 1, &record) == 0);
    assert(record.x == 0x0160 && record.y == 0x0580 && record.kind == -16);
    assert(fa18_projection_grid_record(&grid, 2, &record) == -1);
    assert(fa18_projection_grid_record(&grid, 0, 0) == -1);
    FA18ProjectionPairInput source_pairs[3];
    assert(fa18_projection_grid_pair_source(&grid, -20, source_pairs) == 0);
    assert(source_pairs[0].x == 43 && source_pairs[0].y == -43);
    assert(source_pairs[1].x == -43 && source_pairs[1].y == -43);
    assert(source_pairs[2].x == 0 && source_pairs[2].y == 32);
    assert(fa18_projection_grid_pair_source(&grid, -8, source_pairs) == -1);

    FA18ProjectionGridSetup setup = { 0 };
    assert(fa18_prepare_projection_grid(&grid, -125, -6207, -6578, &setup) == 0);
    assert(setup.record_count == 2 && setup.bounds_limit == 3);
    assert(setup.grid_x == -63 && setup.grid_y == -434);
    assert(setup.scaled_input == -1000 && setup.coordinate_shift == 3);
    assert(fa18_prepare_projection_grid(&grid, -129, -6207, -6578, &setup) == 1);
    assert(fa18_prepare_projection_grid(0, -125, -6207, -6578, &setup) == -1);

    FA18ProjectionGridPacketState packet_state = { 0 };
    FA18ProjectionGridPacketRoute packet_route;
    assert(fa18_initialize_projection_grid_packet(
               &grid, 0, -512, -125, -6207, -6578, &packet_state, &setup,
               &packet_route) == 0);
    assert(packet_state.renderer_state_words[0] == 4 &&
           packet_state.renderer_state_words[1] == 0 &&
           packet_state.renderer_state_words[2] == 0 &&
           packet_state.renderer_state_words[3] == -1);
    assert(packet_state.renderer_selector == 3 &&
           packet_state.line_emitter_mode_flag == 1 &&
           packet_state.negative_kind_flag == 0);
    assert(packet_route == FA18_PROJECTION_GRID_PACKET_READY);
    assert(fa18_complete_projection_grid_packet(&packet_state) == 0);
    assert(packet_state.line_emitter_mode_flag == 0);
    packet_state.line_emitter_mode_flag = 7;
    packet_state.negative_kind_flag = 6;
    assert(fa18_initialize_projection_grid_packet(
               &grid, 1, 0, 0, 0, 0, &packet_state, &setup,
               &packet_route) == 0);
    assert(packet_route == FA18_PROJECTION_GRID_PACKET_MODE_CONTINUATION);
    assert(packet_state.line_emitter_mode_flag == 7 &&
           packet_state.negative_kind_flag == 6);
    assert(fa18_initialize_projection_grid_packet(
               &grid, 0, -2049, 0, 0, 0, &packet_state, &setup,
               &packet_route) == 0);
    assert(packet_route == FA18_PROJECTION_GRID_PACKET_DEPTH_REJECT);
    assert(fa18_initialize_projection_grid_packet(
               &grid, 0, -513, 0, 0, 0, &packet_state, &setup,
               &packet_route) == 0);
    assert(packet_route == FA18_PROJECTION_GRID_PACKET_LOWER_RANGE_CONTINUATION &&
           packet_state.line_emitter_mode_flag == 1 &&
           packet_state.negative_kind_flag == 0);
    assert(fa18_initialize_projection_grid_packet(
               &grid, 0, -512, -129, 0, 0, &packet_state, &setup,
               &packet_route) == 0);
    assert(packet_route == FA18_PROJECTION_GRID_PACKET_ALTERNATE_CONTINUATION);
    assert(fa18_initialize_projection_grid_packet(
               0, 0, 0, 0, 0, 0, &packet_state, &setup, &packet_route) == -1);
    assert(fa18_complete_projection_grid_packet(0) == -1);

    table[4] = 0x01; table[5] = 0x00;
    table[6] = 0x02; table[7] = 0x00;
    table[8] = 0xff; table[9] = 0xf4;
    memset(data + FA18_C279_PROJECTION_BOUNDS_OFFSET, 0,
           FA18_C279_PROJECTION_BOUNDS_BYTES);
    FA18ProjectionGridPreparedRecord prepared;
    FA18ProjectionGridSetup record_setup = { 2, 3, 0, 0, 0, 3 };
    data[FA18_C279_PROJECTION_BOUNDS_OFFSET + 65] = 1;
    assert(fa18_prepare_projection_grid_record(&grid, &record_setup, 0, 0, &prepared) == 1);
    assert(prepared.shifted_x == 0x0800 && prepared.shifted_y == 0x1000);
    assert(prepared.kind == -12 && prepared.bound == 1);
    data[FA18_C279_PROJECTION_BOUNDS_OFFSET + 65] = 2;
    assert(fa18_prepare_projection_grid_record(&grid, &record_setup, 0, 0, &prepared) == 1);
    assert(prepared.kind == 1 && prepared.bound == 2);
    data[FA18_C279_PROJECTION_BOUNDS_OFFSET + 65] = 4;
    assert(fa18_prepare_projection_grid_record(&grid, &record_setup, 0, 0, &prepared) == 0);
    assert(fa18_prepare_projection_grid_record(&grid, &record_setup, 2, 0, &prepared) == -1);

    /* run075 global frame 384, first `$C27B9C` pair: matrix `$C45BD8`,
     * translated pair D3/D4, and base D1/D5/D7 from `-$20(A6)`. */
    FA18ProjectionPairMatrix matrix = {
        { 167, 0, -8, 0, 252, 0, 6, 0, 127 }
    };
    FA18ProjectionPairBase base;
    assert(fa18_prepare_projection_pair_base(&matrix, -1000, &base) == 0);
    assert(base.x == 0 && base.y == -985 && base.depth == 0);
    assert(fa18_prepare_projection_pair_base(0, -1000, &base) == -1);
    FA18ProjectionPairInput pair = { 2355, 7749 };
    FA18ProjectionPairOutput projected;
    assert(fa18_transform_projection_pair(&matrix, &base, &pair, &projected) == 0);
    assert(projected.x == 1294 && projected.y == -985 && projected.depth == 3899);
    assert(fa18_transform_projection_pair(0, &base, &pair, &projected) == -1);

    FA18ProjectionPairScreenPoint screen;
    assert(fa18_project_projection_pair(&projected, &screen) == 1);
    assert(screen.x == 106 && screen.y == 111);
    projected.depth = 0;
    assert(fa18_project_projection_pair(&projected, &screen) == 0);
    projected.x = 101; projected.y = 0; projected.depth = 100;
    assert(fa18_project_projection_pair(&projected, &screen) == 0);
    assert(fa18_project_projection_pair(0, &screen) == -1);

    FA18ProjectionGridPreparedRecord translation = { 2312, 7792, -20, 0 };
    FA18ProjectionTriangle triangle;
    assert(fa18_project_projection_triangle(&matrix, &base, &translation,
                                             source_pairs, &triangle) == 1);
    assert(triangle.points[0].x == 106 && triangle.points[0].y == 111);
    assert(triangle.points[1].x == 109 && triangle.points[1].y == 111);
    assert(triangle.points[2].x == 108 && triangle.points[2].y == 111);
    source_pairs[0].x = 32767;
    assert(fa18_project_projection_triangle(&matrix, &base, &translation,
                                             source_pairs, &triangle) == 0);

    FA18ProjectionGridEmission emission;
    translation.kind = -20;
    assert(fa18_emit_projection_grid_record(&grid, &matrix, &base, &translation,
                                            179, &emission) == FA18_PROJECTION_GRID_TRIANGLE);
    assert(emission.triangle.points[0].x == 106 && emission.triangle.points[2].x == 108);
    translation.kind = 2;
    assert(fa18_emit_projection_grid_record(&grid, &matrix, &base, &translation,
                                            179, &emission) == FA18_PROJECTION_GRID_DIRECT_RENDERER_B);
    assert(emission.direct_pair.x == 108 && emission.direct_pair.y == 111);
    assert(fa18_emit_projection_grid_record(&grid, &matrix, &base, &translation,
                                            110, &emission) == FA18_PROJECTION_GRID_SKIP);
    assert(fa18_emit_projection_grid_record(0, &matrix, &base, &translation,
                                            179, &emission) == -1);
    C302C4AxisAdapter direct_primary = { 0 };
    C302C4AxisAdapter direct_adjacent = { 0 };
    assert(fa18_submit_projection_grid_direct_pair(
               FA18_PROJECTION_GRID_DIRECT_RENDERER_B, emission.direct_pair,
               emit_c302c4_axis, emit_c302c4_axis, &direct_adjacent) == 0);
    assert(direct_adjacent.calls == 1);
    assert(direct_adjacent.x == 108 && direct_adjacent.y == 111);
    assert(fa18_submit_projection_grid_direct_pair(
               FA18_PROJECTION_GRID_DIRECT_RENDERER_A, emission.direct_pair,
               emit_c302c4_axis, emit_c302c4_axis, &direct_primary) == 0);
    assert(direct_primary.calls == 1);
    assert(direct_primary.x == 108 && direct_primary.y == 111);
    assert(fa18_submit_projection_grid_direct_pair(
               FA18_PROJECTION_GRID_TRIANGLE, emission.direct_pair,
               emit_c302c4_axis, emit_c302c4_axis, &direct_primary) == -1);
    FA18ProjectionPairBounds bounds;
    assert(fa18_reduce_projection_pair_bounds(emission.triangle.points, 3, &bounds) == 0);
    assert(bounds.min_x == 106 && bounds.max_x == 109);
    assert(bounds.min_y == 111 && bounds.max_y == 111);
    assert(fa18_reduce_projection_pair_bounds(emission.triangle.points, 2, &bounds) == -1);

    FA18ProjectionPairBoundsRoute route;
    assert(fa18_select_projection_pair_bounds_route(&bounds, 179, &route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_BLITTER_LINE_C2FA7E);
    FA18IndexedFrameBuffer framebuffer;
    memset(&framebuffer, 4, sizeof framebuffer);
    const FA18LineStyle line_style = {0x0f, -1, 0, 0x06};
    C301F6LineAdapter line_adapter = {&framebuffer, &line_style};
    assert(fa18_submit_projection_pair_bounds(&bounds, 179,
                                              emit_c301f6_blitter_line,
                                              &line_adapter, &route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_BLITTER_LINE_C2FA7E);
    for (int x = 0; x < FA18_WIDTH; ++x)
        assert(framebuffer.pixels[112 * FA18_WIDTH + x] ==
               (x >= 106 && x <= 109 ? 6 : 4));

    bounds = (FA18ProjectionPairBounds){100, 104, 20, 23};
    assert(fa18_select_projection_pair_bounds_route(&bounds, 179, &route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION);
    bounds = (FA18ProjectionPairBounds){100, 104, 20, 22};
    assert(fa18_select_projection_pair_bounds_route(&bounds, 179, &route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_C302EC_CONTINUATION);
    bounds = (FA18ProjectionPairBounds){100, 102, 20, 22};
    assert(fa18_select_projection_pair_bounds_route(&bounds, 179, &route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_C2F66E_HELPER);
    bounds = (FA18ProjectionPairBounds){100, 101, 20, 20};
    assert(fa18_select_projection_pair_bounds_route(&bounds, 179, &route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_C302C4_AXIS_STEP);
    bounds = (FA18ProjectionPairBounds){100, 104, 180, 183};
    assert(fa18_select_projection_pair_bounds_route(&bounds, 179, &route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_RETURN);
    assert(fa18_submit_projection_pair_bounds(&bounds, 179, 0, 0, &route) == 0);
    assert(fa18_select_projection_pair_bounds_route(0, 179, &route) == -1);

    C2FF48DmaAdapter dma = { 0 };
    FA18ProjectionPairScreenPoint direct_tuple_list[3] = {
        { 10, 20 }, { 14, 20 }, { 12, 21 }
    };
    assert(fa18_submit_projection_pair_tuple_list(direct_tuple_list, 3, 179,
                                                   emit_c2ff48_dma, &dma,
                                                   emit_c301f6_blitter_line,
                                                   &line_adapter, &route) == 0);
    assert(dma.calls == 1 && dma.value == 0x8400);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_BLITTER_LINE_C2FA7E);
    assert(fa18_submit_projection_pair_tuple_list(direct_tuple_list, 2, 179,
                                                   emit_c2ff48_dma, &dma,
                                                   0, 0, &route) == -1);
    assert(dma.calls == 2 && dma.value == 0x8400);

    C3029ELineAdapter protected_line = {0};
    assert(fa18_submit_projection_pair_protected_line(1, 2, 3, 4, 0,
                                                       0x12345678u,
                                                       emit_c3029e_line,
                                                       &protected_line) == 0);
    assert(protected_line.calls == 1 && protected_line.segment.min_x == 1);
    assert(protected_line.segment.max_x == 3 && protected_line.segment.min_y == 2);
    assert(protected_line.segment.max_y == 4 && protected_line.scratch == 0x000fffffu);
    assert(fa18_submit_projection_pair_protected_line(1, 2, 3, 4, 1,
                                                       0x12345678u,
                                                       emit_c3029e_line,
                                                       &protected_line) == 0);
    assert(protected_line.calls == 2 && protected_line.scratch == 0x12345678u);
    assert(fa18_submit_projection_pair_protected_line(1, 2, 3, 4, 0,
                                                       0, 0, 0) == -1);

    C302C4AxisAdapter first_axis = {0}, second_axis = {0};
    int16_t advanced_y;
    FA18ProjectionPairAxisRoute axis_route;
    assert(fa18_submit_projection_pair_axis_step(20, 30, 178, 0, 179,
                                                  emit_c302c4_axis,
                                                  emit_c302c4_axis,
                                                  &first_axis, &advanced_y,
                                                  &axis_route) == 0);
    assert(axis_route == FA18_PROJECTION_PAIR_AXIS_C2F5F4);
    assert(advanced_y == 179 && first_axis.calls == 1);
    assert(first_axis.x == 20 && first_axis.y == 179);
    assert(second_axis.calls == 0);

    assert(fa18_submit_projection_pair_axis_step(20, 30, 178, 1, 179,
                                                  emit_c302c4_axis,
                                                  emit_c302c4_axis,
                                                  &second_axis, &advanced_y,
                                                  &axis_route) == 0);
    assert(axis_route == FA18_PROJECTION_PAIR_AXIS_C2F60A);
    assert(advanced_y == 179 && second_axis.calls == 1);
    assert(second_axis.x == 30 && second_axis.y == 179);

    assert(fa18_submit_projection_pair_axis_step(20, 30, 179, -1, 179,
                                                  0, 0, 0, &advanced_y,
                                                  &axis_route) == 0);
    assert(axis_route == FA18_PROJECTION_PAIR_AXIS_RETURN);
    assert(advanced_y == 180);
    assert(fa18_submit_projection_pair_axis_step(20, 30, 178, 0, 179,
                                                  0, emit_c302c4_axis,
                                                  &first_axis, &advanced_y,
                                                  &axis_route) == -1);
    assert(fa18_submit_projection_pair_axis_step(20, 30, 178, 1, 179,
                                                  emit_c302c4_axis, 0,
                                                  &second_axis, &advanced_y,
                                                  &axis_route) == -1);

    FA18ProjectionPairRangeState range_state;
    FA18ProjectionPairRangeRoute range_route;
    assert(fa18_prepare_projection_pair_range(16, 10, 100, 10, 179,
                                              &range_state, &range_route) == 0);
    assert(range_route == FA18_PROJECTION_PAIR_RANGE_EQUAL_RETURN);
    assert(fa18_prepare_projection_pair_range(16, 10, 100, 9, 179,
                                              &range_state, &range_route) == 0);
    assert(range_route == FA18_PROJECTION_PAIR_RANGE_C305D6_CONTINUATION);
    assert(fa18_prepare_projection_pair_range(16, 10, 100, 12, 179,
                                              &range_state, &range_route) == 0);
    assert(range_route == FA18_PROJECTION_PAIR_RANGE_C305F8_CONTINUATION);
    assert(range_state.d0 == 2 && range_state.d1 == 88);
    assert(range_state.d3 == 12);
    assert(range_state.d4 == 84 && range_state.d5 == 1);
    assert(range_state.d6 == 16 && range_state.d7 == 442);
    assert(range_state.a1 == 11);
    FA18ProjectionPairBlitterWrites range_blit;
    FA18ProjectionPairBlitterRoute range_blit_route;
    assert(fa18_submit_projection_pair_range_blitter(&range_state, 179, 0x1000,
                                                      &range_blit,
                                                      &range_blit_route) == 0);
    assert(range_blit_route == FA18_PROJECTION_PAIR_BLITTER_C30668_SUBMIT);
    assert(range_blit.bltcon0 == 0x0b4a && range_blit.bltcon1 == 0x0053);
    assert(range_blit.bltapt_low == 0xff5c);
    assert(range_blit.bltamod == 0xfeb4 && range_blit.bltbmod == 4);
    assert(range_blit.bltcpt == 0x11bau && range_blit.bltdpt == 0x11bau);
    assert(range_blit.bltsize == 0x1542);
    assert(fa18_submit_projection_pair_range(16, 10, 100, 12, 179, 0x1000,
                                              &range_blit, &range_route,
                                              &range_blit_route) == 0);
    assert(range_route == FA18_PROJECTION_PAIR_RANGE_C305F8_CONTINUATION);
    assert(range_blit_route == FA18_PROJECTION_PAIR_BLITTER_C30668_SUBMIT);
    assert(range_blit.bltcon0 == 0x0b4a && range_blit.bltsize == 0x1542);
    assert(fa18_submit_projection_pair_range(319, 106, 319, 0, 144, 0x12bc0,
                                              &range_blit, &range_route,
                                              &range_blit_route) == 0);
    assert(range_route == FA18_PROJECTION_PAIR_RANGE_C305D6_CONTINUATION);
    assert(range_blit_route == FA18_PROJECTION_PAIR_BLITTER_C30668_SUBMIT);
    assert(range_blit.bltcon0 == 0xfb4a && range_blit.bltsize == 0x1a82);
    assert(fa18_submit_projection_pair_range(1, 2, 3, 2, 179, 0,
                                              &range_blit, &range_route,
                                              &range_blit_route) == 0);
    assert(range_route == FA18_PROJECTION_PAIR_RANGE_EQUAL_RETURN);
    assert(range_blit_route == FA18_PROJECTION_PAIR_BLITTER_RETURN);
    assert(fa18_prepare_projection_pair_range(16, 179, 100, 180, 179,
                                              &range_state, &range_route) == 0);
    assert(range_route == FA18_PROJECTION_PAIR_RANGE_EQUAL_RETURN);
    assert(fa18_prepare_projection_pair_range(16, -1, 100, 0, 179,
                                              &range_state, &range_route) == 0);
    assert(range_route == FA18_PROJECTION_PAIR_RANGE_C305D6_CONTINUATION);
    assert(fa18_prepare_projection_pair_range(0, 1, 2, 3, 4, 0,
                                              &range_route) == -1);

    FA18ProjectionPairBlitterInput blitter_input = {
        319, 106, 319, 0, 144, 0x00012bc0u
    };
    FA18ProjectionPairBlitterWrites prepared_blit;
    FA18ProjectionPairBlitterRoute blitter_route;
    assert(fa18_prepare_projection_pair_blitter(&blitter_input, &prepared_blit,
                                                 &blitter_route) == 0);
    assert(blitter_route == FA18_PROJECTION_PAIR_BLITTER_C30668_SUBMIT);
    assert(prepared_blit.bltcon0 == 0xfb4a && prepared_blit.bltcon1 == 0x0043);
    assert(prepared_blit.bltafwm == 0xffff);
    assert(prepared_blit.bltadat == 0x8000 && prepared_blit.bltbdat == 0xffff);
    assert(prepared_blit.bltamod == 0xfe5c && prepared_blit.bltbmod == 0);
    assert(prepared_blit.bltcmod == 0x0028 && prepared_blit.bltdmod == 0x0028);
    assert(prepared_blit.bltapt_low == 0xff2e);
    assert(prepared_blit.bltcpt == 0x00012c0fu && prepared_blit.bltdpt == 0x00012c0fu);
    assert(prepared_blit.bltsize == 0x1a82);
    blitter_input.d3 = 144;
    assert(fa18_prepare_projection_pair_blitter(&blitter_input, &prepared_blit,
                                                 &blitter_route) == 0);
    assert(blitter_route == FA18_PROJECTION_PAIR_BLITTER_RETURN);
    assert(fa18_prepare_projection_pair_blitter(0, &prepared_blit,
                                                 &blitter_route) == -1);

    const FA18ProjectionPairFinalInput final_input = {
        0, 319, 106, 0, 144, 0x00012bc0u
    };
    FA18ProjectionPairFinalState final_state;
    assert(fa18_finalize_projection_pair_blit(&final_input, &final_state) == 0);
    assert(final_state.offset_long == 0x000010b6u);
    assert(final_state.lane_long == 0x00013c76u);
    assert(final_state.lane_copy == 0x00013c76u);
    assert(final_state.blit_size == 0x1ad4);
    assert(final_state.bltcon0 == 0x09f0 && final_state.bltcon1 == 0x000a);
    assert(final_state.bltapt == 0x00013c76u);
    assert(final_state.bltcpt == 0xffffffffu && final_state.bltdpt == 0x00013c76u);
    assert(final_state.bltadat == 1 && final_state.bltbdat == 1 &&
           final_state.bltcdat == 1);
    assert(fa18_finalize_projection_pair_blit(0, &final_state) == -1);

    const FA18ProjectionPairScreenPoint final_pairs[] = {
        {10, 20}, {30, 40}, {50, 60}
    };
    FA18ProjectionPairFinalizationInput finalization_input = {
        0, 0, 319, 106, 319, final_pairs, 3, 144, 106, 0, 0x00012bc0u
    };
    C302E6RangeAdapter range_adapter = {0};
    FA18ProjectionPairFinalizationRoute finalization_route;
    assert(fa18_finalize_projection_pair_list(&finalization_input,
                                               emit_c302e6_range, &range_adapter,
                                               &final_state,
                                               &finalization_route) == 0);
    assert(finalization_route == FA18_PROJECTION_PAIR_FINALIZATION_SUBMITTED);
    assert(range_adapter.calls == 3);
    assert(range_adapter.calls_seen[0].min_x == 10 && range_adapter.calls_seen[0].min_y == 20);
    assert(range_adapter.calls_seen[0].max_x == 30 && range_adapter.calls_seen[0].max_y == 40);
    assert(range_adapter.calls_seen[1].min_x == 30 && range_adapter.calls_seen[1].max_x == 50);
    assert(range_adapter.calls_seen[2].min_x == 50 && range_adapter.calls_seen[2].max_x == 10);
    assert(final_state.lane_long == 0x00013c76u && final_state.blit_size == 0x1ad4);
    finalization_input.d6 = 1;
    assert(fa18_finalize_projection_pair_list(&finalization_input, 0, 0,
                                               &final_state,
                                               &finalization_route) == 0);
    assert(finalization_route == FA18_PROJECTION_PAIR_FINALIZATION_C3029E_FALLBACK);

    const FA18ProjectionPairScreenPoint far_list[] = {
        {177, 81}, {206, 101}, {214, 125}, {178, 105}
    };
    C301FarListAdapter far_adapter = {0};
    assert(fa18_submit_projection_pair_far_list(
               far_list, 4, 179, 125, 81, 0x00006048u, 0, 0xdeadbeefu,
               emit_c3029e_line, &far_adapter,
               emit_c301_far_blitter, emit_c301_far_final, &far_adapter,
               &route, &finalization_route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION);
    assert(finalization_route == FA18_PROJECTION_PAIR_FINALIZATION_SUBMITTED);
    assert(far_adapter.blitter_calls > 0 && far_adapter.final_calls == 1);
    assert(far_adapter.final_state.offset_long == 0x000013a2u);
    assert(far_adapter.final_state.lane_long == 0x000073eau);
    assert(far_adapter.final_state.blit_size == 0x0b43);

    /* run036 frame-7000 `$C2FF48` packet.  The second edge reaches the
     * `$C30634-$C30638` quotient-low-bit rounding path. */
    const FA18ProjectionPairScreenPoint run036_pairs[] = {
        {97, 127}, {130, 138}, {74, 145}, {57, 130}
    };
    C301FarListAdapter run036_adapter = {0};
    assert(fa18_submit_projection_pair_far_list(
               run036_pairs, 4, 144, 145, 127, 0x00006048u, 0, 0,
               emit_c3029e_line, &run036_adapter,
               emit_c301_far_blitter, emit_c301_far_final, &run036_adapter,
               &route, &finalization_route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION);
    assert(finalization_route == FA18_PROJECTION_PAIR_FINALIZATION_SUBMITTED);
    assert(run036_adapter.blitter_calls == 4 && run036_adapter.final_calls == 1);
    const uint16_t run036_sizes[4] = {0x0882, 0x0c02, 0x0442, 0x0a42};
    const uint16_t run036_controls[4] = {0x1b4a, 0x2b4a, 0x9b4a, 0x1b4a};
    const uint16_t run036_a_low[4] = {0xffe6, 0xffa8, 0x0016, 0xffb8};
    const uint32_t run036_destinations[4] = {0x7454, 0x7610, 0x74c7, 0x7454};
    for (unsigned run036_index = 0; run036_index < 4; ++run036_index) {
        assert(run036_adapter.blitter[run036_index].bltcon0 == run036_controls[run036_index]);
        assert(run036_adapter.blitter[run036_index].bltapt_low == run036_a_low[run036_index]);
        assert(run036_adapter.blitter[run036_index].bltcpt == run036_destinations[run036_index]);
        assert(run036_adapter.blitter[run036_index].bltdpt == run036_destinations[run036_index]);
        assert(run036_adapter.blitter[run036_index].bltsize == run036_sizes[run036_index]);
    }
    assert(run036_adapter.blitter[0].bltcon1 == 0x0053 &&
           run036_adapter.blitter[1].bltcon1 == 0x0057 &&
           run036_adapter.blitter[2].bltcon1 == 0x0013 &&
           run036_adapter.blitter[3].bltcon1 == 0x0057);
    assert(fa18_submit_projection_pair_far_list(
               direct_tuple_list, 3, 179, 106, 0, 0x00012bc0u, 0, 0,
               emit_c3029e_line, &far_adapter,
               emit_c301_far_blitter, emit_c301_far_final, &far_adapter,
               &route, &finalization_route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_BLITTER_LINE_C2FA7E);

    const FA18ProjectionPairScreenPoint tall_narrow_list[] = {
        {10, 20}, {11, 25}, {10, 30}
    };
    C3029ELineAdapter fallback_line = {0};
    assert(fa18_submit_projection_pair_far_list(
               tall_narrow_list, 3, 179, 30, 20, 0x00006048u, 0, 0xdeadbeefu,
               emit_c3029e_line, &fallback_line, emit_c301_far_blitter, emit_c301_far_final,
               &fallback_line, &route, &finalization_route) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION);
    assert(finalization_route == FA18_PROJECTION_PAIR_FINALIZATION_C3029E_FALLBACK);
    assert(fallback_line.calls == 1 && fallback_line.segment.min_x == 10 &&
           fallback_line.segment.min_y == 20 && fallback_line.segment.max_x == 11 &&
           fallback_line.segment.max_y == 30 && fallback_line.scratch == 0x000fffffu);

    C2FF48DmaAdapter dispatch_dma = {0};
    C301FarListAdapter dispatch_far = {0};
    C3029ELineAdapter dispatch_fallback = {0};
    const FA18ProjectionPairSubmission dispatch = {
        179, 125, 81, 0x00006048u, 0, 0xdeadbeefu,
        emit_c2ff48_dma, &dispatch_dma,
        emit_c301f6_blitter_line, &line_adapter,
        emit_c3029e_line, &dispatch_fallback,
        emit_c301_far_blitter, emit_c301_far_final, &dispatch_far
    };
    assert(fa18_submit_projection_pair_list(far_list, 4, &dispatch, &route,
                                             &finalization_route) == 0);
    assert(dispatch_dma.calls == 1 && dispatch_dma.value == 0x8400);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION);
    assert(finalization_route == FA18_PROJECTION_PAIR_FINALIZATION_SUBMITTED);
    assert(dispatch_far.blitter_calls > 0 && dispatch_far.final_calls == 1);
    assert(fa18_submit_projection_pair_list(tall_narrow_list, 3, &dispatch,
                                             &route, &finalization_route) == 0);
    assert(dispatch_dma.calls == 2 && dispatch_fallback.calls == 1);
    assert(finalization_route == FA18_PROJECTION_PAIR_FINALIZATION_C3029E_FALLBACK);

    translation.kind = -20;
    assert(fa18_submit_projection_grid_record(
               &grid, &matrix, &base, &translation, 179, &dispatch, &emission,
               &route, &finalization_route) == FA18_PROJECTION_GRID_TRIANGLE);
    assert(dispatch_dma.calls == 3);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_BLITTER_LINE_C2FA7E);
    translation.kind = 2;
    assert(fa18_submit_projection_grid_record(
               &grid, &matrix, &base, &translation, 179, &dispatch, &emission,
               &route, &finalization_route) == FA18_PROJECTION_GRID_DIRECT_RENDERER_B);
    assert(dispatch_dma.calls == 3);

    /* `$C27AF4-$C27D0F`: one negative three-pair record followed by a
     * direct renderer-B record. */
    table[4] = 0x01; table[5] = 0x21;
    table[6] = 0x03; table[7] = 0xce;
    table[8] = 0xff; table[9] = 0xec;
    table[10] = 0x01; table[11] = 0x21;
    table[12] = 0x03; table[13] = 0xce;
    table[14] = 0; table[15] = 2;
    FA18ProjectionGridSetup traversal_setup = { 2, 3, 0, 0, -1000, 3 };
    C27GridTraversalAdapter traversal_adapter = { 0 };
    const FA18ProjectionGridSubmission traversal_submission = {
        &dispatch, emit_c27_primary_axis, emit_c27_adjacent_axis, &traversal_adapter,
        fa18_complete_projection_grid_packet, &packet_state
    };
    uint16_t submitted_records = 0;
    assert(fa18_submit_projection_grid_pass(
               &grid, &traversal_setup, &matrix, 0, 179,
               &traversal_submission, &submitted_records) == 0);
    assert(submitted_records == 2);
    assert(dispatch_dma.calls == 4);
    assert(traversal_adapter.primary_calls == 0 &&
           traversal_adapter.adjacent_calls == 1);
    assert(packet_state.line_emitter_mode_flag == 0);
    traversal_setup.record_count = 1;
    assert(fa18_submit_projection_grid_pass(
               &grid, &traversal_setup, &matrix, 0, 179,
               &traversal_submission, &submitted_records) == -1);
    finalization_input.d6 = 2;
    assert(fa18_finalize_projection_pair_list(&finalization_input, 0, 0,
                                               &final_state,
                                               &finalization_route) == -1);

    table[0] = 0xff; table[1] = 0xff;
    assert(fa18_load_projection_grid(&hunks, &grid) == -1);
    assert(fa18_load_projection_grid(0, &grid) == -1);
    return 0;
}
