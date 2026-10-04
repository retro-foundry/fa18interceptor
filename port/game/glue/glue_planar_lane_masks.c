/* Renderer dispatch-table targets. The mode is the table index in the ROM. */
#include "glue.h"
#include "ports_glue.h"

#include "planar_lane_masks.h"

#define LANE_HANDLER(address, mode, rows)                         \
    int glue_##address(void) {                                    \
        const gaddr words[4] = {A(3), A(2), A(1), A(0)};           \
        const uint16_t clear[4] = {(uint16_t)D(0), (uint16_t)D(1), \
                                   (uint16_t)D(2), (uint16_t)D(3)}; \
        const uint16_t set[4] = {(uint16_t)D(4), (uint16_t)D(5),   \
                                 (uint16_t)D(6), (uint16_t)D(7)}; \
        uint16_t result = apply_planar_lane_masks(mode, rows, words, clear, set); \
        flags_logic_w(result);                                    \
        return glue_return();                                     \
    }

/* $C2F786: one row, selectors 0..15. */
LANE_HANDLER(C2F826,  0, 0)
LANE_HANDLER(C2F83A,  1, 0)
LANE_HANDLER(C2F844,  2, 0)
LANE_HANDLER(C2F84E,  3, 0)
LANE_HANDLER(C2F858,  4, 0)
LANE_HANDLER(C2F862,  5, 0)
LANE_HANDLER(C2F86C,  6, 0)
LANE_HANDLER(C2F876,  7, 0)
LANE_HANDLER(C2F880,  8, 0)
LANE_HANDLER(C2F88A,  9, 0)
LANE_HANDLER(C2F894, 10, 0)
LANE_HANDLER(C2F89E, 11, 0)
LANE_HANDLER(C2F8A8, 12, 0)
LANE_HANDLER(C2F8B2, 13, 0)

/* $C2F7E6: the same masks at the current word and the next scanline. */
LANE_HANDLER(C2F8EA,  1, 1)
LANE_HANDLER(C2F904,  2, 1)
LANE_HANDLER(C2F91E,  3, 1)
LANE_HANDLER(C2F96C,  6, 1)
LANE_HANDLER(C2F986,  7, 1)
LANE_HANDLER(C2F9A0,  8, 1)
LANE_HANDLER(C2F9BA,  9, 1)
LANE_HANDLER(C2F9D4, 10, 1)
LANE_HANDLER(C2F9EE, 11, 1)
LANE_HANDLER(C2FA08, 12, 1)
LANE_HANDLER(C2FA22, 13, 1)

LANE_HANDLER(C2F8BC, 14, 0)

LANE_HANDLER(C2F8C6, 15, 0)

LANE_HANDLER(C2F8D0, 0, 1)

LANE_HANDLER(C2F938, 4, 1)

LANE_HANDLER(C2F952, 5, 1)

LANE_HANDLER(C2FA3C, 14, 1)

LANE_HANDLER(C2FA56, 15, 1)
