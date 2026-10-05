#ifndef FA18_MAP_PACKET_CONTROL_WALKER_H
#define FA18_MAP_PACKET_CONTROL_WALKER_H

#include "map_packet_record_stage.h"

typedef int (*FA18MapPacketRecordInputResolver)(void *context, uint8_t mode,
    FA18MapPacketRecordStageInput *input);

typedef struct {
    const uint8_t *stream;
    size_t stream_size;
    FA18MapPacketRecordInputResolver resolve_record;
    void *context;
} FA18MapPacketControlWalkerInput;

typedef enum {
    FA18_MAP_PACKET_CONTROL_TERMINATOR = 0,
    FA18_MAP_PACKET_CONTROL_FRAME_STOP = 1
} FA18MapPacketControlWalkerRoute;

/* `$C2AD00` loop, bounded by the caller-owned control-stream size: decode
 * control bytes, preserve the negative-byte frame flag, and execute each
 * resolved record stage until the original terminator or frame-stop path. */
int fa18_walk_map_packet_controls(const FA18MapPacketControlWalkerInput *input,
                                  FA18MapPacketProjectionRecord *records,
                                  size_t record_capacity,
                                  uint16_t *record_count,
                                  FA18MapPacketControlWalkerRoute *route);

#endif
