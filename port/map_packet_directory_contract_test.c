#include "map_packet_directory.h"

#include <assert.h>

int main(void) {
    FA18MapPacketDirectoryState state;
    assert(fa18_initialize_map_packet_directory(
               FA18_MAP_PACKET_DIRECTORY_NORMAL, &state) == 0);
    assert(state.layout == FA18_MAP_PACKET_DIRECTORY_NORMAL &&
           !state.frame_flag && state.record_base_address == UINT32_C(0x00c42ca8) &&
           state.coordinate_bin_shift == 0x0c && state.row_max == 7 &&
           state.column_max == 7);
    assert(fa18_initialize_map_packet_directory(
               FA18_MAP_PACKET_DIRECTORY_WIDE, &state) == 0);
    assert(state.layout == FA18_MAP_PACKET_DIRECTORY_WIDE &&
           !state.frame_flag && state.record_base_address == UINT32_C(0x00c42e6c) &&
           state.coordinate_bin_shift == 8 && state.row_max == 0x1f &&
           state.column_max == 0x1f);
    assert(fa18_initialize_map_packet_directory(
               (FA18MapPacketDirectoryLayout)2, &state) == -1);
    return 0;
}
