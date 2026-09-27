#include "map_packet_control_walker.h"

int fa18_walk_map_packet_controls(const FA18MapPacketControlWalkerInput *input,
                                  FA18MapPacketProjectionRecord *records,
                                  size_t record_capacity,
                                  uint16_t *record_count,
                                  FA18MapPacketControlWalkerRoute *route) {
    if (!input || !input->stream || !input->resolve_record || !records ||
        !record_count || !route)
        return -1;
    uint8_t frame_flag = 0;
    for (size_t cursor = 0; cursor != input->stream_size; ++cursor) {
        if (frame_flag) {
            *record_count = 0;
            *route = FA18_MAP_PACKET_CONTROL_FRAME_STOP;
            return 0;
        }
        const int8_t encoded_mode = (int8_t)input->stream[cursor];
        if (encoded_mode == -1) {
            *record_count = 0;
            *route = FA18_MAP_PACKET_CONTROL_TERMINATOR;
            return 0;
        }
        FA18MapPacketRecordStageInput record;
        if (input->resolve_record(input->context, (uint8_t)encoded_mode, &record) != 0)
            return -1;
        record.encoded_mode = encoded_mode;
        record.gate.frame_flag = frame_flag;
        FA18MapDetailGateResult gate;
        FA18MapPacketRecordStageRoute record_route;
        if (fa18_run_map_packet_record_stage(&record, records, record_capacity,
                                             record_count, &gate, &record_route) != 0)
            return -1;
        if (record_route == FA18_MAP_PACKET_RECORD_TERMINATOR) {
            *route = FA18_MAP_PACKET_CONTROL_TERMINATOR;
            return 0;
        }
        if (record_route == FA18_MAP_PACKET_RECORD_FRAME_STOP) {
            *route = FA18_MAP_PACKET_CONTROL_FRAME_STOP;
            return 0;
        }
        frame_flag = gate.frame_flag;
    }
    return -1;
}
