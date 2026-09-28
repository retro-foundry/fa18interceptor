#include "renderer_pointer_set_clear.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t chip[512];
    const uint32_t pointers[FA18_RENDERER_POINTER_SET_COUNT]
                           [FA18_RENDERER_POINTER_SET_POINTERS] = {
        {16, 80, 144, 208},
        {272, 336, 400, 464}
    };

    memset(chip, 0xa5, sizeof chip);
    assert(fa18_clear_renderer_pointer_sets(pointers, chip, sizeof chip) == 0);
    for (unsigned set = 0; set < FA18_RENDERER_POINTER_SET_COUNT; ++set)
        for (unsigned pointer = 0; pointer < FA18_RENDERER_POINTER_SET_POINTERS;
             ++pointer)
            for (unsigned offset = 0; offset < FA18_RENDERER_POINTER_SET_CLEAR_BYTES;
                 ++offset)
                assert(chip[pointers[set][pointer] + offset] == 0);
    assert(chip[0] == 0xa5 && chip[56] == 0xa5);

    {
        uint8_t unchanged[sizeof chip];
        uint32_t invalid[FA18_RENDERER_POINTER_SET_COUNT]
                        [FA18_RENDERER_POINTER_SET_POINTERS];
        memcpy(unchanged, chip, sizeof chip);
        memcpy(invalid, pointers, sizeof invalid);
        invalid[1][3] = sizeof chip - FA18_RENDERER_POINTER_SET_CLEAR_BYTES + 1;
        assert(fa18_clear_renderer_pointer_sets(invalid, chip, sizeof chip) == -1);
        assert(memcmp(chip, unchanged, sizeof chip) == 0);
    }
    assert(fa18_clear_renderer_pointer_sets(0, chip, sizeof chip) == -1);
    assert(fa18_clear_renderer_pointer_sets(pointers, 0, sizeof chip) == -1);
    return 0;
}
