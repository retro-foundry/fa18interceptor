#ifndef FA18_MAP_PACKET_PARENT_PASS_H
#define FA18_MAP_PACKET_PARENT_PASS_H

#include "map_packet_depth_stage.h"
#include "map_packet_original_pass.h"

typedef struct {
    FA18MapPacketDepthStageInput depth;
    FA18MapPacketOriginalPassInput pass;
} FA18MapPacketParentPassInput;

typedef struct {
    FA18MapPacketDepthStageResult depth;
    FA18MapPacketPassSelectorResult normal_pass;
    FA18MapPacketPassSelectorResult wide_pass;
    FA18MapPacketControlWalkerRoute normal_route;
    FA18MapPacketControlWalkerRoute wide_route;
    uint16_t normal_record_count;
    uint16_t wide_record_count;
} FA18MapPacketParentPassResult;

/* `$C2AA9C-$C2AB33`: derive the depth metric, optionally run the normal
 * packet pass, then always run the wide packet pass. The record/matrix/page
 * fields in `pass` stay parent-owned. */
int fa18_run_map_packet_parent_pass(
    const FA18MapPacketParentPassInput *input,
    FA18MapPacketProjectionRecord *records, size_t record_capacity,
    FA18MapPacketParentPassResult *result);

#endif
