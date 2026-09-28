#include "map_packet_record_stage.h"

int fa18_run_map_packet_record_stage(
    const FA18MapPacketRecordStageInput *input,
    FA18MapPacketProjectionRecord *records, size_t record_capacity,
    uint16_t *record_count, FA18MapDetailGateResult *gate_result,
    FA18MapPacketRecordStageRoute *route) {
    if (!input || !records || !record_count || !route) return -1;

    FA18MapDetailGateResult gate;
    FA18MapDetailGateRoute gate_route;
    if (fa18_select_map_detail_gate(input->encoded_mode, &input->gate, &gate,
                                    &gate_route) != 0)
        return -1;
    if (gate_route == FA18_MAP_DETAIL_GATE_TERMINATOR) {
        *record_count = 0;
        *route = FA18_MAP_PACKET_RECORD_TERMINATOR;
        return 0;
    }
    if (gate_route == FA18_MAP_DETAIL_GATE_FRAME_STOP) {
        *record_count = 0;
        *route = FA18_MAP_PACKET_RECORD_FRAME_STOP;
        return 0;
    }
    if (gate_result) *gate_result = gate;

    FA18MapPacketStageInput packet_stage = input->packet_stage;
    if (input->use_relative_offset) {
        FA18MapPacketRelativeOffsetInput relative = input->relative_offset;
        relative.mode = gate.mode;
        FA18MapPacketRelativeOffsetResult selected;
        FA18MapPacketRelativeOffsetRoute relative_route;
        if (fa18_select_map_packet_relative_offset(&relative, &selected,
                                                   &relative_route) != 0)
            return -1;
        if (relative_route != FA18_MAP_PACKET_RELATIVE_OFFSET_READY) {
            *record_count = 0;
            *route = FA18_MAP_PACKET_RECORD_REJECTED;
            return 0;
        }
        packet_stage.selector.packet = selected.packet;
        packet_stage.selector.packet_size = selected.packet_size;
    }

    FA18MapDetailFieldsInput fields = input->fields;
    fields.alternate_layout = input->gate.alternate_layout;
    fields.force_visible = gate.detail_byte;
    fields.visibility_gate = gate.visibility_flag;
    fields.detail_metric = input->gate.metric;
    FA18MapDetailFieldsResult detail;
    if (fa18_apply_map_detail_fields(&fields, &detail) != 0) return -1;

    packet_stage.selector.alternate_stream = detail.visible != 0;
    packet_stage.transform.detail_shift = gate.coordinate_shift;
    packet_stage.coordinate_shift = gate.coordinate_shift;
    packet_stage.transform.packed_seed =
        fa18_complete_map_detail_component_route(detail.coordinate_x,
                                                 detail.coordinate_y);

    FA18MapPacketStageRoute packet_route;
    if (fa18_run_map_packet_stage(&packet_stage, records, record_capacity,
                                  record_count, &packet_route) != 0)
        return -1;
    if (packet_route == FA18_MAP_PACKET_STAGE_DISPLAYED)
        *route = FA18_MAP_PACKET_RECORD_DISPLAYED;
    else if (packet_route == FA18_MAP_PACKET_STAGE_REJECTED)
        *route = FA18_MAP_PACKET_RECORD_REJECTED;
    else
        *route = FA18_MAP_PACKET_RECORD_COUNT_ERROR;
    return 0;
}
