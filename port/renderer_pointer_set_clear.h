#ifndef FA18_RENDERER_POINTER_SET_CLEAR_H
#define FA18_RENDERER_POINTER_SET_CLEAR_H

#include <stddef.h>
#include <stdint.h>

enum {
    FA18_RENDERER_POINTER_SET_COUNT = 2,
    FA18_RENDERER_POINTER_SET_POINTERS = 4,
    FA18_RENDERER_POINTER_SET_CLEAR_BYTES = 40
};

/* `$C2F582-$C2F5BF`: clear each 40-byte target selected by the base and
 * offset four-pointer renderer sets. Pointer values are caller-owned offsets
 * into the supplied Chip-RAM image, never recorded Amiga addresses. */
int fa18_clear_renderer_pointer_sets(
    const uint32_t pointer_sets[FA18_RENDERER_POINTER_SET_COUNT]
                               [FA18_RENDERER_POINTER_SET_POINTERS],
    uint8_t *chip_bytes, size_t chip_byte_count);

#endif
