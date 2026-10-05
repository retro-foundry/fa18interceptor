#ifndef FA18_MAP_PACKET_PASS_SELECTOR_H
#define FA18_MAP_PACKET_PASS_SELECTOR_H

#include "map_packet_column_table.h"
#include "map_packet_coordinate_setup.h"
#include "map_packet_directory.h"
#include "map_packet_low_filter.h"
#include "map_packet_wide_control_stream.h"

typedef struct {
    FA18MapPacketDirectoryLayout layout;
    FA18MapPacketCoordinateSetupInput coordinate;
    int32_t metric;
    int8_t metric_selector;
    int8_t table_selector;
    FA18MapPacketLowFilterRowResolver resolve_low_filter_row;
    void *context;
} FA18MapPacketPassSelectorInput;

typedef struct {
    FA18MapPacketDirectoryState directory;
    FA18MapPacketCoordinateSetupResult coordinate;
    uint32_t control_stream_address;
} FA18MapPacketPassSelectorResult;

/* `$C2AB34-$C2AD00`, excluding the caller-owned byte walker: initialize one
 * map pass, derive its coordinate state, and choose its next control stream. */
int fa18_select_map_packet_pass(const FA18MapPacketPassSelectorInput *input,
                                FA18MapPacketPassSelectorResult *result);

#endif
