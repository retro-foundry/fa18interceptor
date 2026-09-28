#include "context_selector_pack.h"

#include <assert.h>

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8); bytes[1] = (uint8_t)value;
}
static int origin(void *context, int32_t value[3]) {
    (void)context;
    value[0] = 0x10800000; value[1] = 0; value[2] = 0x11000000;
    return 0;
}
int main(void) {
    uint8_t record[FA18_CONTEXT_SELECTOR_RECORD_BYTES] = {0};
    FA18ContextSelectorPackState state = {0};
    put16(record + 6, 1); put16(record + 8, 2); record[0x0a] = 0x7f;
    put16(record + 0x56, 0x20); put16(record + 0x58, 0x40);
    put16(record + 0x5a, 0x100); put16(record + 0x6c, 0x1001);
    assert(fa18_update_context_selector_pack(record, sizeof record, &state,
                                             0x0b, 0, 0) == 0);
    assert(state.magnitude_class == 3 && state.selector_word_x == 1 &&
           state.selector_word_y == 2 && state.selector_byte_y == 0x7f &&
           state.selector_byte_x == 6 && state.selector_change == 0x0b);
    state.context_selection = 1;
    state.mode = 2;
    state.projection_depth = -0x1000;
    assert(fa18_update_context_selector_pack(record, sizeof record, &state,
                                             0, origin, 0) == 0);
    assert(state.origin[0] == 0x10800000 && state.origin[2] == 0x11000000 &&
           state.selector_word_x == 0x42 && state.selector_word_y == 0x44 &&
           state.selector_byte_y == 15 && state.selector_byte_x == 13 &&
           state.selector_change == 0xff);
    return 0;
}
