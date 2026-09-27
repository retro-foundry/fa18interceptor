#ifndef FA18_MAP_PACKET_STAGE_H
#define FA18_MAP_PACKET_STAGE_H

#include "map_packet_selector.h"
#include "map_packet_transform.h"

typedef int (*FA18MapPacketDisplayStage)(
    void *context, const FA18MapPacketProjectionRecord *records,
    uint16_t record_count);

typedef struct {
    FA18MapPacketSelectorInput selector;
    FA18MapPacketTransform transform;
    FA18MapPacketDisplayStage display_stage;
    void *display_context;
} FA18MapPacketStageInput;

typedef enum {
    FA18_MAP_PACKET_STAGE_DISPLAYED = 0,
    FA18_MAP_PACKET_STAGE_REJECTED = 1,
    FA18_MAP_PACKET_STAGE_COUNT_ERROR = 2
} FA18MapPacketStageRoute;

/* `$C2AEFC-$C2AFE2`: select a packet stream, decode its original big-endian
 * pair records, build `$C4BF92`-style projection records, then invoke the
 * caller-owned `$C246A0` display-stage boundary. */
int fa18_run_map_packet_stage(const FA18MapPacketStageInput *input,
                              FA18MapPacketProjectionRecord *records,
                              size_t record_capacity,
                              uint16_t *record_count,
                              FA18MapPacketStageRoute *route);

#endif
