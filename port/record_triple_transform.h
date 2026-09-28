#ifndef FA18_RECORD_TRIPLE_TRANSFORM_H
#define FA18_RECORD_TRIPLE_TRANSFORM_H

#include <stddef.h>
#include <stdint.h>

/* `$C1F4AC-$C1F578`: direct raw-triple portion of the `$C1EE14` route.
 * The caller owns the surrounding descriptor/count gates and the following
 * `$C1F6F8` record-walker handoff. */
typedef struct {
    const int16_t *source_words; /* consecutive `(A1)+` triples */
    size_t source_word_count;
    uint16_t triple_count;
    uint16_t source_shift; /* `-8(A6)` */
    int16_t translate_x; /* D0.w */
    int16_t translate_y; /* A5.w */
    int16_t translate_z; /* D1.w */
    const int16_t *matrix_words; /* nine `$C45BD8` words */
    int16_t *destination_words; /* `(A3)+`, six bytes per output triple */
    size_t destination_word_capacity;
} FA18RecordTripleTransformInput;

int fa18_transform_record_triples(const FA18RecordTripleTransformInput *input);

#endif
