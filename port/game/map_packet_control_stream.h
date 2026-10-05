#ifndef FA18_MAP_PACKET_CONTROL_STREAM_H
#define FA18_MAP_PACKET_CONTROL_STREAM_H

#include <stdint.h>

typedef struct {
    int16_t local_x;
    int16_t local_y;
    int32_t metric;
} FA18MapPacketControlStreamInput;

typedef struct {
    uint16_t cell;
    uint32_t stream_address;
} FA18MapPacketControlStreamResult;

/* `$C2ABDE-$C2AC18`: select the normal-layout control-byte stream from the
 * two saved coordinate words and the `$5000` metric branch. */
int fa18_select_normal_map_packet_control_stream(
    const FA18MapPacketControlStreamInput *input,
    FA18MapPacketControlStreamResult *result);

#endif
