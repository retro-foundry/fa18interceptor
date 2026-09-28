#ifndef FA18_MAP_PACKET_DEPTH_STAGE_H
#define FA18_MAP_PACKET_DEPTH_STAGE_H

#include <stdint.h>

#include "projection_packet.h"

typedef struct {
    const FA18ProjectionPacket *packet;
    uint8_t initial_bounds_ready;
    uint8_t metric_scale_inhibit;
    uint16_t metric_scale;
} FA18MapPacketDepthStageInput;

typedef struct {
    int16_t renderer_words[4];
    int32_t metric;
    uint8_t run_normal_pass;
    uint8_t run_wide_pass;
} FA18MapPacketDepthStageResult;

/* `$C2AA9C-$C2AB33`: initialize the map renderer words, derive the
 * projection-depth metric from `$C45A78`, and select normal/wide pass calls.
 * `packet->depth_metric` is the complete value published by `$C1C636`. */
int fa18_prepare_map_packet_depth_stage(
    const FA18MapPacketDepthStageInput *input,
    FA18MapPacketDepthStageResult *result);

#endif
