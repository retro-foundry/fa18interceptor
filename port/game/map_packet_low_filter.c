#include "map_packet_low_filter.h"

static uint32_t select_table(int32_t metric) {
    if (metric <= INT32_C(0x10)) return UINT32_C(0x00c2aa5c);
    if (metric <= INT32_C(0x160)) return UINT32_C(0x00c2aa1c);
    return UINT32_C(0x00c2a9dc);
}

int fa18_run_map_packet_low_filter(const FA18MapPacketLowFilterInput *input,
                                   FA18MapPacketLowFilterResult *result,
                                   FA18MapPacketLowFilterRoute *route) {
    if (!input || !result || !route) return -1;
    result->stream_address = 0;
    result->detail_flag = 0;
    if (input->metric > INT32_C(0x3a0)) {
        *route = FA18_MAP_PACKET_LOW_FILTER_COLUMN_STAGE;
        return 0;
    }
    if (!input->resolve_row) return -1;
    int8_t row[4];
    if (input->resolve_row(input->context, select_table(input->metric),
                           (int16_t)input->table_selector, row) != 0)
        return -1;
    const int8_t selector = input->metric_selector;
    if (row[0] < 0 || row[0] == selector || row[1] == selector ||
        row[2] == selector || row[3] == selector) {
        result->stream_address = UINT32_C(0x00c2aca8);
        result->detail_flag = 1;
        *route = FA18_MAP_PACKET_LOW_FILTER_MATCH;
    } else {
        *route = FA18_MAP_PACKET_LOW_FILTER_COLUMN_STAGE;
    }
    return 0;
}
