#ifndef FA18_MAP_PACKET_WIDE_CONTROL_STREAM_H
#define FA18_MAP_PACKET_WIDE_CONTROL_STREAM_H

#include <stdint.h>

typedef enum {
    FA18_MAP_PACKET_WIDE_LOW_FILTER = 0,
    FA18_MAP_PACKET_WIDE_MIDDLE_STREAM = 1,
    FA18_MAP_PACKET_WIDE_HIGH_STREAM = 2
} FA18MapPacketWideControlStreamRoute;

typedef struct {
    uint32_t stream_address;
} FA18MapPacketWideControlStreamResult;

/* `$C2AC1A-$C2AC3D`: select a wide-layout control stream by the two exact
 * depth thresholds.  The low branch transfers to later `$C2AC3E` filtering. */
int fa18_select_wide_map_packet_control_stream(
    int32_t metric, FA18MapPacketWideControlStreamResult *result,
    FA18MapPacketWideControlStreamRoute *route);

#endif
