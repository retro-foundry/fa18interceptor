#ifndef FA18_MAP_PACKET_COLUMN_TABLE_H
#define FA18_MAP_PACKET_COLUMN_TABLE_H

#include <stdint.h>

typedef struct {
    int8_t table_selector;
    int16_t metric_selector;
    int32_t metric;
} FA18MapPacketColumnTableInput;

typedef struct {
    uint32_t stream_address;
} FA18MapPacketColumnTableResult;

/* `$C2ACAA-$C2AD00`: calculate a control-stream address in one of the three
 * static column-table banks using original word-width arithmetic. */
int fa18_select_map_packet_column_table(const FA18MapPacketColumnTableInput *input,
                                        FA18MapPacketColumnTableResult *result);

#endif
