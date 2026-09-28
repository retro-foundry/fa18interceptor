#include "default_scene_map_pass.h"
#include "run075_trig_asset.h"
#include "two_angle_matrix.h"

#include <assert.h>
#include <string.h>

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}
static void put32(uint8_t *bytes, uint32_t value) {
    put16(bytes, (uint16_t)(value >> 16));
    put16(bytes + 2, (uint16_t)value);
}
typedef struct { unsigned calls; } Fixture;
static int display(void *context, const FA18MapPacketProjectionRecord *records,
                   uint16_t count, uint16_t shift) {
    Fixture *fixture = context;
    if (!fixture || !records || count != 1 || shift != 0) return -1;
    ++fixture->calls;
    return 0;
}
static int low_row(void *context, uint32_t address, int16_t selector,
                   int8_t row[4]) {
    (void)context;
    if (address != 0x00c2aa1c || selector != 6) return -1;
    row[0] = -1; row[1] = row[2] = row[3] = 0;
    return 0;
}

int main(void) {
    enum { LOW_STREAM_OFFSET = 0xda8, WIDE_DIRECTORY_OFFSET = 0x1c4,
           PACKET_OFFSET = 0x1e4 };
    uint8_t hunk63[0xae8 + sizeof fa18_run075_trig_bytes] = {0};
    FA18HunkSegment segments[64] = {{0}};
    FA18Hunks hunks = {segments, 64};
    FA18FlightTrigTable trig = {0};
    uint8_t scene_record[0xa4] = {0};
    uint8_t grid_records[6] = {0, 0, 0, 0, 0, 2};
    uint8_t bounds[0x400] = {0};
    FA18ProjectionGrid grid = {1, 0x100, grid_records, bounds, {0}};
    FA18FivePlanePage page;
    FA18FlightRendererPage renderer;
    FA18PlanarPixelState pixels = {0};
    FA18LineStyle lines = {0};
    uint8_t control[0xe00] = {0};
    uint8_t packet[0x240] = {0};
    FA18MapPacketStaticData static_data = {control, sizeof control,
                                            packet, sizeof packet};
    Fixture fixture = {0};
    FA18DefaultSceneMapPassInput input = {0};
    FA18DefaultSceneMapPassResult result;
    FA18MapPacketProjectionRecord records[0x12];

    memcpy(hunk63 + 0xae8, fa18_run075_trig_bytes, sizeof fa18_run075_trig_bytes);
    segments[63] = (FA18HunkSegment){.kind = FA18_HUNK_CODE, .data = hunk63,
                                     .size = sizeof hunk63};
    assert(fa18_load_two_angle_trig_table(&hunks, &trig) == 0);
    put16(scene_record + 0x66, 0); put16(scene_record + 0x68, 28600);
    put16(scene_record + 0x6a, 0);
    put32(scene_record + 0x14, 0); put32(scene_record + 0x18, 0x80);
    put32(scene_record + 0x1c, 0);
    /* `$C1C54E` transforms seed (0,4,18); this makes its Y contribution
     * -3, so the source root at +$18 yields the frame-382-shaped -125 depth. */
    put16(scene_record + 0x9a, (uint16_t)-48);
    control[LOW_STREAM_OFFSET] = 4; control[LOW_STREAM_OFFSET + 1] = 0xff;
    packet[WIDE_DIRECTORY_OFFSET + 1] = 0x20;
    packet[PACKET_OFFSET + 5] = 1;
    packet[PACKET_OFFSET + 7] = 1;
    packet[PACKET_OFFSET + 9] = 2;
    fa18_five_plane_page_init(&page);
    assert(fa18_flight_renderer_page_init(&renderer, &page, &pixels, &lines,
                                           179, 111, 106, 0, 0, 0) == 0);
    input.scene = (FA18DefaultSceneRenderPassInput){
        {scene_record, sizeof scene_record, 0, 0}, {0, 0, 0, {168,252,128}},
        0, 200};
    input.selector_pack = (FA18ContextSelectorPackState){
        0, 0, 0, {0,0,0}, 0, 0, 6, 0, 0, 0};
    input.map.depth = (FA18MapPacketDepthStageInput){0, 0, 1, 0x80};
    input.map.pass.static_data = &static_data;
    input.map.pass.selector = (FA18MapPacketPassSelectorInput){
        FA18_MAP_PACKET_DIRECTORY_WIDE, {0,{0,0,0},{0,0,0},0,0},
        0, 0, 0, low_row, 0};
    input.map.pass.record.encoded_mode = 0;
    input.map.pass.record.gate = (FA18MapDetailGateInput){0,0,0,0,0};
    input.map.pass.record.fields = (FA18MapDetailFieldsInput){
        0,0,0,0,1,0x80,0,0,0,0};
    input.map.pass.record.packet_stage.selector = (FA18MapPacketSelectorInput){
        packet + PACKET_OFFSET, sizeof packet - PACKET_OFFSET, 0, 0,
        {0,0,0},0,0,0,0};
    input.map.pass.record.packet_stage.transform = (FA18MapPacketTransform){
        0,{0,0,0},0,{{256,0,0,0,256,0,0,0,256}}};
    input.map.pass.record.packet_stage.display_stage = display;
    input.map.pass.record.packet_stage.display_context = &fixture;
    assert(fa18_render_default_active_scene_map_pass(
        &trig, &grid, &input, &renderer, records, 0x12, &result) == 0);
    assert(result.scene.scene_pipeline.packet.depth_metric == -125 &&
           result.map.depth.metric == 125 && !result.map.depth.run_normal_pass &&
           result.map.wide_route == FA18_MAP_PACKET_CONTROL_TERMINATOR &&
           fixture.calls == 1);
    assert(!memcmp(result.scene.matrix_route.second_matrix,
                   (int16_t[3][3]){{167,0,-8},{0,252,0},{6,0,127}},
                   sizeof result.scene.matrix_route.second_matrix));
    return 0;
}
