/* Live-game adapter for the source-backed map packet selector and walker.
 * Both original entries join at $C2AB7C. Static tables are read from the
 * loaded Slow RAM image; mutable frame fields and polygon input stay in the
 * game memory image shared with the remaining translated callers. */
#include "map_packet.h"

#include "globals.h"
#include "machine.h"

#include "../map_packet_original_pass.h"

typedef struct {
    gaddr frame;
    gaddr component;
    FA18MapPacketStaticData data;
    FA18MapPacketOriginalPassInput original;
    FA18MapPacketPassSelectorResult pass;
    MapPacketRegisterEffects *effects;
    uint32_t control_cursor;
    int16_t origin_x;
} MapPacketRuntime;

static void set_word(uint32_t *reg, uint16_t value) {
    *reg = (*reg & 0xffff0000u) | value;
}

static void source_data(FA18MapPacketStaticData *data) {
    const uint32_t control = FA18_MAP_PACKET_CONTROL_RUNTIME_BASE - FA18_SLOW_BASE;
    const uint32_t packet = FA18_MAP_PACKET_PACKET_RUNTIME_BASE - FA18_SLOW_BASE;
    data->control_bytes = fa18_machine->slow + control;
    data->control_size = FA18_SLOW_SIZE - control;
    data->packet_bytes = fa18_machine->slow + packet;
    data->packet_size = FA18_SLOW_SIZE - packet;
}

static int display_packet(void *context,
                          const FA18MapPacketProjectionRecord *records,
                          uint16_t count, uint16_t shift) {
    unsigned i, k;
    MapPacketRuntime *runtime = context;
    uint32_t saved_d0 = runtime->effects->data[0];
    wr_u16(CLIP_INPUT, shift);
    wr_u16(CLIP_INPUT + 2, count);
    for (i = 0; i < count; ++i)
        for (k = 0; k < 3; ++k)
            wr_s16(CLIP_INPUT + 4 + (gaddr)(6 * i + 2 * k), records[i].value[k]);
    runtime->effects->address[5] = CLIP_INPUT + 4 + 6u * count;
    /* MOVEM.W from memory sign-extends into a data register on 68000. */
    runtime->effects->data[6] = (uint32_t)(int32_t)runtime->origin_x;
    if (runtime->effects->draw_polygon(runtime->effects->draw_context,
                                       runtime->effects) != 0)
        return -1;
    runtime->effects->data[0] = saved_d0; /* $C2AFE8 restores D0. */
    return 0;
}

static void publish_origin(void *context, const int16_t origin[3]) {
    MapPacketRuntime *runtime = context;
    unsigned i;
    for (i = 0; i < 3; ++i)
        wr_s16(runtime->frame - 8 + (gaddr)(2 * i), origin[i]);
    runtime->origin_x = origin[0];
}

static void publish_cursor(void *context, const uint8_t *next_word,
                           int16_t last_word, uint8_t detail_cutoff) {
    MapPacketRuntime *runtime = context;
    runtime->effects->address[3] = FA18_MAP_PACKET_PACKET_RUNTIME_BASE +
        (gaddr)(next_word - runtime->data.packet_bytes);
    if (detail_cutoff) {
        uint16_t relative = (uint16_t)(((uint16_t)last_word & 0x7fffu) << 2);
        runtime->effects->data[1] = (uint32_t)(int32_t)(int16_t)relative;
    } else {
        set_word(&runtime->effects->data[1], (uint16_t)last_word);
    }
}

static void publish_packet(void *context, uint32_t packet_address) {
    MapPacketRuntime *runtime = context;
    runtime->effects->address[3] = packet_address;
}

static void publish_seed(void *context, uint32_t packed_seed) {
    MapPacketRuntime *runtime = context;
    runtime->effects->data[0] = packed_seed;
}

static void publish_transform(void *context, uint32_t last_y_register) {
    MapPacketRuntime *runtime = context;
    runtime->effects->data[7] = last_y_register;
}

static void publish_visibility_limit(void *context, uint32_t limit_register) {
    MapPacketRuntime *runtime = context;
    runtime->effects->data[6] = limit_register;
}

static void prepare_control_registers(MapPacketRuntime *runtime, uint8_t mode,
                                      const int8_t pair[2]) {
    MapPacketRegisterEffects *effects = runtime->effects;
    int16_t row = (int16_t)(uint16_t)((uint16_t)runtime->pass.coordinate.row_min +
                                     (uint16_t)(int16_t)pair[0]);
    int16_t column = (int16_t)(uint16_t)((uint16_t)runtime->pass.coordinate.column_min +
                                        (uint16_t)(int16_t)pair[1]);
    uint16_t shifted_column;
    uint16_t index;

    effects->address[0] = runtime->pass.control_stream_address +
                          ++runtime->control_cursor;
    effects->address[2] = FA18_MAP_PACKET_CONTROL_RUNTIME_BASE + 2u * mode;
    set_word(&effects->data[2], (uint16_t)(2u * mode));
    set_word(&effects->data[3], (uint16_t)(int16_t)pair[0]);
    set_word(&effects->data[0], (uint16_t)row);
    if (row < 0 || row > runtime->pass.directory.row_max) return;

    set_word(&effects->data[3], (uint16_t)(int16_t)pair[1]);
    set_word(&effects->data[1], (uint16_t)column);
    if (column < 0 || column > runtime->pass.directory.column_max) return;

    shifted_column = (uint16_t)((uint16_t)column <<
        (runtime->pass.directory.layout == FA18_MAP_PACKET_DIRECTORY_WIDE ? 6u : 4u));
    index = (uint16_t)((uint16_t)row * 2u + shifted_column);
    set_word(&effects->data[0], index);
    set_word(&effects->data[1], shifted_column);
    set_word(&effects->data[0],
             rd_u16(runtime->pass.directory.record_base_address + index));
    effects->address[1] = runtime->pass.directory.record_base_address;
    effects->address[3] = runtime->pass.directory.record_base_address;
}

static int runtime_record(void *context, uint8_t encoded,
                          FA18MapPacketRecordStageInput *record) {
    MapPacketRuntime *runtime = context;
    FA18MapDetailGateResult gate;
    FA18MapDetailGateRoute route;
    int8_t pair[2];
    uint8_t mode = encoded & 0x7f;
    uint32_t mask = runtime->pass.directory.layout ==
        FA18_MAP_PACKET_DIRECTORY_WIDE ? 0x00ffffffu : 0x0fffffffu;
    unsigned i;

    if (fa18_prepare_original_map_packet_record(&runtime->original,
            &runtime->pass, mode, record) != 0 ||
        fa18_resolve_map_packet_control_pair(&runtime->data, mode, pair) != 0)
        return -1;

    prepare_control_registers(runtime, mode, pair);

    record->gate = (FA18MapDetailGateInput){
        0, runtime->pass.directory.layout == FA18_MAP_PACKET_DIRECTORY_WIDE,
        rd_u8(0xC457ADu), rd_u8(0xC4589Cu), rd_s32(runtime->frame - 0x28)
    };
    if (fa18_select_map_detail_gate((int8_t)encoded, &record->gate,
                                    &gate, &route) != 0 ||
        route != FA18_MAP_DETAIL_GATE_READY)
        return -1;
    wr_u16(runtime->frame - 0x44, gate.frame_flag);
    wr_u16(runtime->frame - 0x22, gate.visibility_flag);
    wr_u16(runtime->frame - 0x20, gate.coordinate_shift);
    wr_u8(runtime->frame - 0x24, gate.detail_byte);
    if (!gate.detail_byte && gate.visibility_flag)
        runtime->effects->data[6] = 0; /* $C2AE76: MOVEQ #0,D6 */

    record->fields = (FA18MapDetailFieldsInput){0};
    record->fields.zoom_endpoint = rd_u8(0xC457DDu);
    record->fields.zoom_scale = rd_s16(0xC45A42u);
    record->fields.coordinate_x = (int32_t)(0u - (rd_u32(runtime->component) & mask));
    record->fields.coordinate_y = (int32_t)(0u - (rd_u32(runtime->component + 8) & mask));
    record->fields.offset_x = (int32_t)((uint32_t)(uint8_t)pair[0] << 24);
    record->fields.offset_y = (int32_t)((uint32_t)(uint8_t)pair[1] << 24);

    record->packet_stage.selector.packet_origin = rd_s32(runtime->component + 4);
    record->packet_stage.selector.origin_adjusted =
        runtime->pass.directory.layout == FA18_MAP_PACKET_DIRECTORY_WIDE;
    record->packet_stage.selector.detail_metric = rd_s32(runtime->frame - 0x28);
    for (i = 0; i < 3; ++i) {
        record->packet_stage.selector.origin_matrix[i] =
            rd_s16(VIEW_ANGLE_MATRIX + 2 + (gaddr)(6 * i));
        record->packet_stage.transform.matrix.value[3 * i] =
            rd_s16(VIEW_ANGLE_MATRIX + (gaddr)(6 * i));
        record->packet_stage.transform.matrix.value[3 * i + 1] =
            rd_s16(VIEW_ANGLE_MATRIX + 2 + (gaddr)(6 * i));
        record->packet_stage.transform.matrix.value[3 * i + 2] =
            rd_s16(VIEW_ANGLE_MATRIX + 4 + (gaddr)(6 * i));
    }
    record->packet_stage.selector.resolve_stream =
        fa18_resolve_map_packet_static_packet;
    record->packet_stage.selector.context = &runtime->data;
    record->packet_stage.workspace_shift = rd_u16(CLIP_INPUT);
    record->packet_stage.display_stage = display_packet;
    record->packet_stage.display_context = runtime;
    record->packet_stage.publish_origin = publish_origin;
    record->packet_stage.publish_cursor = publish_cursor;
    record->publish_packet = publish_packet;
    record->publish_seed = publish_seed;
    record->publish_visibility_limit = publish_visibility_limit;
    record->packet_stage.publish_transform = publish_transform;
    return 0;
}

int run_map_packet_pass(gaddr frame, int wide, MapPacketRegisterEffects *effects) {
    MapPacketRuntime runtime = {0};
    FA18MapPacketControlWalkerInput walker;
    FA18MapPacketControlWalkerRoute route;
    FA18MapPacketProjectionRecord records[0x12];
    const uint8_t *stream;
    size_t stream_size;
    uint16_t count;
    unsigned i;

    if (!effects || !effects->draw_polygon) return -1;
    runtime.frame = frame;
    runtime.effects = effects;
    runtime.component = rd_u8(0xC45785u) ? 0xC45C3Eu :
        0xC46184u + (gaddr)(int32_t)rd_s16(0xC458DEu) + 0x14u;
    source_data(&runtime.data);
    runtime.original.static_data = &runtime.data;
    runtime.original.allow_negative_packet = rd_u8(0xC457B0u);
    runtime.original.selector.layout = wide ? FA18_MAP_PACKET_DIRECTORY_WIDE :
                                              FA18_MAP_PACKET_DIRECTORY_NORMAL;
    runtime.original.selector.coordinate.directory_selector_gate = rd_u8(0xC45785u);
    for (i = 0; i < 3; ++i) {
        runtime.original.selector.coordinate.control_component[i] =
            rd_s32(runtime.component + (gaddr)(4 * i));
        runtime.original.selector.coordinate.selector_component[i] =
            rd_s32(0xC45C3Eu + (gaddr)(4 * i));
    }
    runtime.original.selector.metric = rd_s32(frame - 0x28);
    runtime.original.selector.metric_selector = rd_s8(0xC45854u);
    runtime.original.selector.table_selector = rd_s8(0xC45850u);
    runtime.original.selector.resolve_low_filter_row =
        fa18_resolve_map_packet_low_filter_row;
    runtime.original.selector.context = &runtime.data;

    wr_u16(frame - 0x3e, wide ? 1 : 0);
    wr_u16(frame - 0x44, 0);
    wr_u32(frame - 0x34, wide ? 0xC42E6Cu : 0xC42CA8u);
    wr_u16(frame - 0x40, wide ? 8 : 12);
    wr_u16(frame - 0x30, wide ? 31 : 7);
    wr_u16(frame - 0x2e, wide ? 31 : 7);
    if (fa18_select_map_packet_pass(&runtime.original.selector,
                                     &runtime.pass) != 0)
        return -1;
    effects->address[4] = runtime.component;
    effects->address[0] = runtime.pass.control_stream_address;
    wr_s16(frame - 2, runtime.pass.coordinate.origin_component);
    wr_s16(frame - 0x2c, runtime.pass.coordinate.row_min);
    wr_s16(frame - 0x2a, runtime.pass.coordinate.column_min);
    if (wide && runtime.pass.control_stream_address == 0xC2ACA8u)
        wr_u8(0xC4589Du, 1);
    if (fa18_resolve_map_packet_control_stream(&runtime.data,
            runtime.pass.control_stream_address, &stream, &stream_size) != 0)
        return -1;
    walker = (FA18MapPacketControlWalkerInput){
        stream, stream_size, runtime_record, &runtime
    };
    if (fa18_walk_map_packet_controls(&walker, records, 0x12,
                                      &count, &route) != 0)
        return -1;
    effects->address[0] = runtime.pass.control_stream_address +
                          runtime.control_cursor +
                          (route == FA18_MAP_PACKET_CONTROL_TERMINATOR);
    if (route == FA18_MAP_PACKET_CONTROL_TERMINATOR)
        set_word(&effects->data[2], 0xffffu);
    effects->address[4] = runtime.component;
    return 0;
}
