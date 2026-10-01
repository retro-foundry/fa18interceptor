/* Four-plane word writes selected by the renderer's two handler tables. */
#include "planar_lane_masks.h"

uint16_t apply_planar_lane_masks(unsigned set_lanes, int two_rows,
                                 const gaddr plane_words[4],
                                 const uint16_t clear_masks[4],
                                 const uint16_t set_masks[4]) {
    uint16_t result = 0;
    unsigned lane;
    for (lane = 0; lane < 4; ++lane) {
        const int set = (set_lanes & (1u << lane)) != 0;
        const uint16_t mask = set ? set_masks[lane] : clear_masks[lane];
        const gaddr address = plane_words[lane];
        uint16_t value = rd_u16(address);
        result = set ? (uint16_t)(value | mask) : (uint16_t)(value & mask);
        wr_u16(address, result);
        if (two_rows) {
            value = rd_u16(address + 0x28u);
            result = set ? (uint16_t)(value | mask) : (uint16_t)(value & mask);
            wr_u16(address + 0x28u, result);
        }
    }
    return result;
}
