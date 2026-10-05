#ifndef FA18_TEMPLATE_BITMASK_BUFFERS_H
#define FA18_TEMPLATE_BITMASK_BUFFERS_H

#include <stddef.h>
#include <stdint.h>

#include "hunk.h"
#include "field_bytes.h"

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

typedef struct {
    const uint8_t *streams[3];
    size_t stream_sizes[3];
    FA18TemplateBitmaskBuffers *buffers;
    /* Actual adjacent data owners, ending immediately before / beginning
     * immediately after the three gate buffers. Signed bit indices can reach
     * these fields. Supply only resolved canonical fields, never padding. */
    const PortFieldByte *before,*after;
    size_t before_count,after_count;
    uint16_t *error_word;
} FA18TemplateBitmaskState;

/* Complete C1C40C, including source fault words and the actual RTS fault hook.
 * Return 1 on source completion (including a reported source error), 0 on
 * missing data/owner, retaining preceding writes. A negative list skips;
 * odd positive lengths truncate, while lengths 0/1 consume 65536 words. */
int fa18_run_template_bitmask_state(FA18TemplateBitmaskState *state);
/* Attach live original Hunk-66 streams without clearing buffers/other fields. */
int fa18_bind_template_bitmask_streams(FA18TemplateBitmaskState *state,
                                      const FA18Hunks *hunks);

/* `$C1C40C-$C1C54D`: clear and expand the three compact Hunk-66 streams into
 * the mutable `$C1929C/$C19A9C/$C1A29C` bit-gate buffers.  A nonzero result
 * is the source error word `$43/$44/$45` for streams one/two/three; -1 means
 * missing data/owner, not a source fault. This wrapper supplies no neighbours. */
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
