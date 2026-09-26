#include "projection_packet.h"

#include <stdint.h>

static int32_t add_wrap(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t negate_wrap(int32_t value) {
    return (int32_t)(UINT32_C(0) - (uint32_t)value);
}

static int16_t low_word_after_shift(int32_t value) {
    const int32_t shifted = value < 0 ? (value - 255) / 256 : value / 256;
    return (int16_t)(uint16_t)shifted;
}

int fa18_publish_projection_packet(const FA18ProjectionRoot *root,
                                   FA18ProjectionInput input,
                                   FA18ProjectionPacket *packet) {
    if (!root || !packet) return -1;
    const int32_t x = negate_wrap(add_wrap(input.x, root->x & INT32_C(0x003fffff)));
    const int32_t y = negate_wrap(add_wrap(input.y, root->y));
    const int32_t z = negate_wrap(add_wrap(input.z, root->z & INT32_C(0x003fffff)));
    packet->x = low_word_after_shift(x);
    packet->y = low_word_after_shift(y);
    packet->z = low_word_after_shift(z);
    packet->depth_metric = y;
    return 0;
}
