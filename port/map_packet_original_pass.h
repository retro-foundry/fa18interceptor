#ifndef FA18_MAP_PACKET_ORIGINAL_PASS_H
#define FA18_MAP_PACKET_ORIGINAL_PASS_H

#include "map_packet_pass_runner.h"
#include "map_packet_static_data.h"

typedef struct {
    const FA18MapPacketStaticData *static_data;
    FA18MapPacketPassSelectorInput selector;
    /* Live state prepared by the parent; this adapter supplies only original
     * static directory/control/packet bytes. */
    FA18MapPacketRecordStageInput record;
    uint8_t allow_negative_packet;
} FA18MapPacketOriginalPassInput;

/* `$C2AB34-$C2AFF9`: compose one source-shaped map pass using Hunk 28/68
 * resolvers. The parent still owns all live coordinate, matrix, detail, and
 * page-submission state in `record`. */
int fa18_run_original_map_packet_pass(
    const FA18MapPacketOriginalPassInput *input,
    FA18MapPacketProjectionRecord *records, size_t record_capacity,
    uint16_t *record_count, FA18MapPacketPassSelectorResult *pass,
    FA18MapPacketControlWalkerRoute *route);

#endif
