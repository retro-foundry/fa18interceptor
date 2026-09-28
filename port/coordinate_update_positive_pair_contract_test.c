#include "coordinate_update_positive_pair.h"

#include <assert.h>

static void put_word(uint8_t *bytes, unsigned index, uint16_t value) {
    bytes[index * 2u] = (uint8_t)(value >> 8);
    bytes[index * 2u + 1u] = (uint8_t)value;
}

int main(void) {
    uint8_t words[128] = {0};
    const FA18CoordinateAngleTable table = {words, sizeof words};
    const FA18SceneCoordinateUpdateInput input = {0, 0, 0x00800000, 0,
                                                   0x0b000000, -1};
    int16_t output[2] = {0};

    /* At `$C1253A`, the source's rounded ratio is 745; the `$C12540` ASR #6
     * selects table word 11.  Its Hunk-63 value is 25, yielding
     * `$7080 - (25 << 3) = $6FB8`. */
    put_word(words, 11, 25);
    put_word(words, 0, 0);
    assert(fa18_update_coordinate_positive_pair(&table, &input, output) == 0);
    assert(output[0] == 0 && output[1] == 0x6fb8);
    { FA18SceneCoordinateUpdateInput rejected = input; rejected.d5 = 0;
      assert(fa18_update_coordinate_positive_pair(&table, &rejected, output) == -1); }
    return 0;
}
