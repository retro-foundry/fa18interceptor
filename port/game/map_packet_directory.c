#include "map_packet_directory.h"

int fa18_initialize_map_packet_directory(FA18MapPacketDirectoryLayout layout,
                                         FA18MapPacketDirectoryState *state) {
    if (!state) return -1;
    if (layout != FA18_MAP_PACKET_DIRECTORY_NORMAL &&
        layout != FA18_MAP_PACKET_DIRECTORY_WIDE)
        return -1;

    state->layout = layout;
    state->frame_flag = 0;
    if (layout == FA18_MAP_PACKET_DIRECTORY_WIDE) {
        state->record_base_address = UINT32_C(0x00c42e6c);
        state->coordinate_bin_shift = 8;
        state->row_max = 0x1f;
        state->column_max = 0x1f;
    } else {
        state->record_base_address = UINT32_C(0x00c42ca8);
        state->coordinate_bin_shift = 0x0c;
        state->row_max = 7;
        state->column_max = 7;
    }
    return 0;
}
