#ifndef FA18_MAP_PACKET_PASS_RUNNER_H
#define FA18_MAP_PACKET_PASS_RUNNER_H

#include "map_packet_control_walker.h"
#include "map_packet_pass_selector.h"

typedef int (*FA18MapPacketControlStreamResolver)(void *context,
                                                   uint32_t address,
                                                   const uint8_t **stream,
                                                   size_t *stream_size);
typedef int (*FA18MapPacketPassRecordResolver)(void *context,
    const FA18MapPacketPassSelectorResult *pass, uint8_t mode,
    FA18MapPacketRecordStageInput *record);

typedef struct {
    FA18MapPacketPassSelectorInput selector;
    FA18MapPacketControlStreamResolver resolve_control_stream;
    FA18MapPacketPassRecordResolver resolve_record;
    void *context;
} FA18MapPacketPassRunnerInput;

/* `$C2AB34-$C2AFF9` composition boundary: select one pass's directory and
 * control-stream address, resolve that original stream, then execute its
 * control records until the original terminator/frame-stop return. */
int fa18_run_map_packet_pass(const FA18MapPacketPassRunnerInput *input,
                             FA18MapPacketProjectionRecord *records,
                             size_t record_capacity, uint16_t *record_count,
                             FA18MapPacketPassSelectorResult *pass,
                             FA18MapPacketControlWalkerRoute *route);

#endif
