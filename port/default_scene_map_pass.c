#include "default_scene_map_pass.h"

#include <string.h>

static int32_t read_be32(const uint8_t *bytes) {
    return (int32_t)((uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
                     (uint32_t)bytes[2] << 8 | bytes[3]);
}

int fa18_render_default_active_scene_map_pass(
    const FA18FlightTrigTable *trig_table, const FA18ProjectionGrid *grid,
    const FA18DefaultSceneMapPassInput *input,
    FA18FlightRendererPage *page_renderer,
    FA18MapPacketProjectionRecord *records, size_t record_capacity,
    FA18DefaultSceneMapPassResult *result) {
    const uint8_t *record;
    FA18MapPacketParentPassInput map;
    int status;

    if (!trig_table || !grid || !input || !page_renderer || !records || !result ||
        fa18_resolve_scene_active_record(
            &input->scene.active_record, FA18_SCENE_PROJECTION_SEED_RECORD_BYTES,
            &record) != 0)
        return -1;
    status = fa18_render_default_active_scene_pass(
        trig_table, grid, &input->scene, page_renderer, &result->scene);
    if (status != 0) return status;

    map = input->map;
    map.depth.packet = &result->scene.scene_pipeline.packet;
    map.pass.selector.coordinate.control_component[0] = read_be32(record + 0x14);
    map.pass.selector.coordinate.control_component[1] = read_be32(record + 0x18);
    map.pass.selector.coordinate.control_component[2] = read_be32(record + 0x1c);
    memcpy(map.pass.record.packet_stage.transform.matrix.value,
           result->scene.matrix_route.second_matrix,
           sizeof map.pass.record.packet_stage.transform.matrix.value);
    return fa18_run_map_packet_parent_pass(&map, records, record_capacity,
                                           &result->map);
}
