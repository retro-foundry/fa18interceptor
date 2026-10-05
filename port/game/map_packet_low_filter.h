#ifndef FA18_MAP_PACKET_LOW_FILTER_H
#define FA18_MAP_PACKET_LOW_FILTER_H

#include <stdint.h>

typedef int (*FA18MapPacketLowFilterRowResolver)(void *context,
    uint32_t table_address, int16_t selector, int8_t row[4]);

typedef struct {
    int8_t metric_selector;
    int8_t table_selector;
    int32_t metric;
    FA18MapPacketLowFilterRowResolver resolve_row;
    void *context;
} FA18MapPacketLowFilterInput;

typedef enum {
    FA18_MAP_PACKET_LOW_FILTER_MATCH = 0,
    FA18_MAP_PACKET_LOW_FILTER_COLUMN_STAGE = 1
} FA18MapPacketLowFilterRoute;

typedef struct {
    uint32_t stream_address;
    uint8_t detail_flag;
} FA18MapPacketLowFilterResult;

/* `$C2AC3E-$C2ACA7`: choose the metric's four-byte selector table, match the
 * signed selector byte against it, and either select `$C2ACA8` or continue
 * into the column-table path. */
int fa18_run_map_packet_low_filter(const FA18MapPacketLowFilterInput *input,
                                   FA18MapPacketLowFilterResult *result,
                                   FA18MapPacketLowFilterRoute *route);

#endif
