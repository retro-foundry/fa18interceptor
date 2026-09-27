#include "map_packet_relative_offset.h"

#include "hunk.h"

static int16_t add_word(int16_t left, int8_t right) {
    return (int16_t)(uint16_t)((uint16_t)left + (uint16_t)(int16_t)right);
}

int fa18_select_map_packet_relative_offset(
    const FA18MapPacketRelativeOffsetInput *input,
    FA18MapPacketRelativeOffsetResult *result,
    FA18MapPacketRelativeOffsetRoute *route) {
    if (!input || !result || !route || !input->record_directory ||
        !input->resolve_control_pair || !input->resolve_packet)
        return -1;

    int8_t pair[2];
    if (input->resolve_control_pair(input->context, input->mode, pair) != 0)
        return -1;
    const int16_t row = add_word(input->row_min, pair[0]);
    if (row < 0 || row > input->row_max) {
        *route = FA18_MAP_PACKET_RELATIVE_OFFSET_OUT_OF_BOUNDS;
        return 0;
    }
    const int16_t column = add_word(input->column_min, pair[1]);
    if (column < 0 || column > input->column_max) {
        *route = FA18_MAP_PACKET_RELATIVE_OFFSET_OUT_OF_BOUNDS;
        return 0;
    }

    const uint16_t row_bytes = (uint16_t)row << 1;
    const uint16_t column_bytes = (uint16_t)column <<
        (input->wide_layout ? 6u : 4u);
    const size_t directory_index = (uint16_t)(row_bytes + column_bytes);
    if (directory_index > input->record_directory_size ||
        input->record_directory_size - directory_index < 2u)
        return -1;
    const int16_t relative = (int16_t)fa18_be16(
        input->record_directory + directory_index);
    if (relative <= 0) {
        result->error_code = 0x40;
        *route = FA18_MAP_PACKET_RELATIVE_OFFSET_INVALID;
        return 0;
    }

    const uint32_t packet_address = input->record_base_address +
        (uint16_t)relative;
    const uint8_t *packet;
    size_t packet_size;
    if (input->resolve_packet(input->context, packet_address, &packet,
                              &packet_size) != 0 || !packet || packet_size < 2u)
        return -1;
    if ((int16_t)fa18_be16(packet) < 0 && !input->allow_negative_packet) {
        *route = FA18_MAP_PACKET_RELATIVE_OFFSET_RETRY;
        return 0;
    }

    result->row = row;
    result->column = column;
    result->packet_address = packet_address;
    result->packet = packet;
    result->packet_size = packet_size;
    result->error_code = 0;
    *route = FA18_MAP_PACKET_RELATIVE_OFFSET_READY;
    return 0;
}
