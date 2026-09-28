#ifndef FA18_TEMPLATE_BITMASK_BUFFERS_H
#define FA18_TEMPLATE_BITMASK_BUFFERS_H

#include <stddef.h>
#include <stdint.h>

#include "hunk.h"

enum {
    FA18_TEMPLATE_BITMASK_BYTES = 0x800,
    FA18_TEMPLATE_BITMASK_ROWS = 0x80,
    FA18_TEMPLATE_BITMASK_ROW_BYTES = 0x10,
    FA18_TEMPLATE_BITMASK_HUNK = 66,
    FA18_TEMPLATE_BITMASK_STREAM_A_OFFSET = 0,
    FA18_TEMPLATE_BITMASK_STREAM_B_OFFSET = 0x100,
    FA18_TEMPLATE_BITMASK_STREAM_C_OFFSET = 0x200
};

typedef struct {
    uint8_t first[FA18_TEMPLATE_BITMASK_BYTES];
    uint8_t second[FA18_TEMPLATE_BITMASK_BYTES];
    uint8_t third[FA18_TEMPLATE_BITMASK_BYTES];
} FA18TemplateBitmaskBuffers;

/* `$C1C40C-$C1C54D`: clear and expand the three compact Hunk-66 streams into
 * the mutable `$C1929C/$C19A9C/$C1A29C` bit-gate buffers.  A nonzero result
 * is the source error word `$43/$44/$45` for streams one/two/three. */
int fa18_build_template_bitmask_buffers(const uint8_t *first_stream,
                                        size_t first_size,
                                        const uint8_t *second_stream,
                                        size_t second_size,
                                        const uint8_t *third_stream,
                                        size_t third_size,
                                        FA18TemplateBitmaskBuffers *buffers);

/* Bind the three compact streams at `$C42290/$C42390/$C42490` directly from
 * original Hunk 66. */
int fa18_initialize_template_bitmask_buffers(const FA18Hunks *hunks,
                                             FA18TemplateBitmaskBuffers *buffers);

#endif
