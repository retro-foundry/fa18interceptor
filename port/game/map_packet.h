#ifndef FA18_GAME_MAP_PACKET_H
#define FA18_GAME_MAP_PACKET_H

#include "memory.h"

/* One normal ($C2AB5A) or wide ($C2AB34) map packet pass. The parent owns
 * the A6 frame and the projection-depth metric at frame -$28. */
int run_map_packet_pass(gaddr frame, int wide);

#endif
