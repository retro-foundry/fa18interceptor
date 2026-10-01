#ifndef FA18_GAME_PLANAR_LANE_MASKS_H
#define FA18_GAME_PLANAR_LANE_MASKS_H

#include "memory.h"

/* $C2F826-$C2FA6F: apply one renderer word mask to each of four planes.
 * Lane 0 is A3, then A2, A1 and A0. The offset table repeats the operation
 * one scanline (+40 bytes) below each first word. Returns the final word
 * written, whose N/Z flags the 68000 leaves at RTS. */
uint16_t apply_planar_lane_masks(unsigned set_lanes, int two_rows,
                                 const gaddr plane_words[4],
                                 const uint16_t clear_masks[4],
                                 const uint16_t set_masks[4]);

#endif
