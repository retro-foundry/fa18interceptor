#ifndef FA18_GAME_MAP_PACKET_H
#define FA18_GAME_MAP_PACKET_H

#include "memory.h"
#include "../map_packet_pass_selector.h"
#include "../map_packet_depth_stage.h"

/* Observations of the packet walk, in source order. The optional observer
 * belongs to the translated-caller bridge; the game never receives CPU state.
 * draw_polygon consumes CLIP_INPUT once at the $C246A0 child boundary. */
typedef struct {
    void *context;
    int (*draw_polygon)(void *context, gaddr projected_end, int16_t origin_x);
    void (*begin)(void *context, gaddr component,
                  const FA18MapPacketPassSelectorResult *pass);
    void (*control)(void *context, uint8_t mode, const int8_t pair[2]);
    void (*cursor)(void *context, gaddr next_word, int16_t last_word,
                   uint8_t detail_cutoff);
    void (*packet)(void *context, gaddr packet);
    void (*seed)(void *context, uint32_t packed_seed);
    void (*transform)(void *context, uint32_t last_y);
    void (*visibility_limit)(void *context, uint32_t limit);
    void (*detail)(void *context, uint8_t detail, uint16_t visibility);
    void (*end)(void *context, gaddr next_control, int terminated);
} MapPacketHooks;

/* One normal ($C2AB5A) or wide ($C2AB34) map packet pass. The parent owns
 * the A6 frame and the projection-depth metric at frame -$28. */
int run_map_packet_pass(gaddr frame, int wide, const MapPacketHooks *hooks);

/* $C2AA9C: publish renderer defaults and the source-selected depth metric
 * into the caller's frame before the normal/wide pass sequence. */
FA18MapPacketDepthStageResult prepare_map_packet_depth(gaddr frame);

#endif
