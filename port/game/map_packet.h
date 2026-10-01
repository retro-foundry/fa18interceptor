#ifndef FA18_GAME_MAP_PACKET_H
#define FA18_GAME_MAP_PACKET_H

#include "memory.h"

typedef struct MapPacketRegisterEffects MapPacketRegisterEffects;
typedef int (*MapPacketPolygonDraw)(void *context, MapPacketRegisterEffects *effects);

/* Register effects of the packet walker. The caller owns the $C246A0
 * child boundary and passes its resulting registers back. */
struct MapPacketRegisterEffects {
    uint32_t data[8];
    uint32_t address[6];
    MapPacketPolygonDraw draw_polygon;
    void *draw_context;
};

/* One normal ($C2AB5A) or wide ($C2AB34) map packet pass. The parent owns
 * the A6 frame and the projection-depth metric at frame -$28. */
int run_map_packet_pass(gaddr frame, int wide, MapPacketRegisterEffects *effects);

#endif
