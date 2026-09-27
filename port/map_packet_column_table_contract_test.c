#include "map_packet_column_table.h"

#include <assert.h>

int main(void) {
    FA18MapPacketColumnTableInput input = {2, 3, 0x3000};
    FA18MapPacketColumnTableResult result;
    assert(fa18_select_map_packet_column_table(&input, &result) == 0);
    assert(result.stream_address == UINT32_C(0x00c2a828));
    input.metric = 0x3001;
    assert(fa18_select_map_packet_column_table(&input, &result) == 0);
    assert(result.stream_address == UINT32_C(0x00c2a54e));
    input.metric = 0x4801;
    assert(fa18_select_map_packet_column_table(&input, &result) == 0);
    assert(result.stream_address == UINT32_C(0x00c2a174));
    input.table_selector = -1;
    input.metric_selector = -1;
    input.metric = 0;
    assert(fa18_select_map_packet_column_table(&input, &result) == 0);
    assert(result.stream_address == UINT32_C(0x00c2a7b8));
    return 0;
}
