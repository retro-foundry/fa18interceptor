#ifndef FA18_C1F99A_RECORD_TRANSFORM_H
#define FA18_C1F99A_RECORD_TRANSFORM_H

#include <stddef.h>
#include <stdint.h>

#include "transform.h"

typedef struct {
    /* The descriptor selected through `$C45A32`; offsets remain source-byte
     * offsets.  `$C1F9BC` and `$C1F9C8` both add descriptor_offset. */
    const uint8_t *descriptor_bytes;
    size_t descriptor_size;
    size_t descriptor_cursor;
    int16_t descriptor_offset; /* D7 at `$C1F99A`. */
    uint16_t record_count;     /* D0 at `$C1F99A`. */
    uint8_t record_shift;      /* `-8(a6)`. */
    uint16_t frame_shift;      /* `-6(a6)`. */
    int32_t prepared_component[3]; /* `-32(a6)`, before `EXG D1,D2`. */
    int32_t stream_component[3]; /* `$C45B30/$34/$38`. */
    FA18TransformMatrix first_matrix;  /* `$C45BC6`. */
    FA18TransformMatrix second_matrix; /* `$C45BD8`. */
    uint8_t *workspace_bytes; /* `$C48390` mutable bytes. */
    size_t workspace_size;
} FA18C1F99ARecordTransformInput;

/* `$C1F99A-$C1FB22`, observed descriptor bit-0 transform route.  A
 * descriptor with bit 0 clear transfers through `$C1FA92` to the separately
 * bounded `$C1FB24` continuation and is rejected here rather than guessed. */
int fa18_transform_c1f99a_record(const FA18C1F99ARecordTransformInput *input);

/* Callback adapter for `$C1F94E`'s direct BSR to `$C1F99A`. */
int fa18_transform_c1f99a_record_callback(void *context, int16_t count,
                                          int16_t descriptor_offset);

#endif
