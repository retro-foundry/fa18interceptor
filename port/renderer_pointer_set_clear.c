#include "renderer_pointer_set_clear.h"

#include <string.h>

int fa18_clear_renderer_pointer_sets(
    const uint32_t pointer_sets[FA18_RENDERER_POINTER_SET_COUNT]
                               [FA18_RENDERER_POINTER_SET_POINTERS],
    uint8_t *chip_bytes, size_t chip_byte_count) {
    if (!pointer_sets || !chip_bytes) return -1;

    /* Validate every MOVEM.L target before the first source store so a bad
     * native binding cannot leave a partially-cleared renderer state. */
    for (unsigned set = 0; set < FA18_RENDERER_POINTER_SET_COUNT; ++set)
        for (unsigned pointer = 0; pointer < FA18_RENDERER_POINTER_SET_POINTERS;
             ++pointer)
            if (pointer_sets[set][pointer] > chip_byte_count ||
                chip_byte_count - pointer_sets[set][pointer] <
                    FA18_RENDERER_POINTER_SET_CLEAR_BYTES)
                return -1;

    for (unsigned set = 0; set < FA18_RENDERER_POINTER_SET_COUNT; ++set)
        for (unsigned pointer = 0; pointer < FA18_RENDERER_POINTER_SET_POINTERS;
             ++pointer)
            memset(chip_bytes + pointer_sets[set][pointer], 0,
                   FA18_RENDERER_POINTER_SET_CLEAR_BYTES);
    return 0;
}
