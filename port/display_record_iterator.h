#ifndef FA18_DISPLAY_RECORD_ITERATOR_H
#define FA18_DISPLAY_RECORD_ITERATOR_H

#include <stdint.h>

#include "display_record_projection.h"

enum {
    FA18_DISPLAY_RECORD_ITERATOR_RECORD_COUNT = 8,
    FA18_DISPLAY_RECORD_ITERATOR_WORDS_PER_RECORD = 8
};

/* `$C2E758-$C2EC67`: the caller supplies the already-selected, 16-byte-stride
 * `$C4B390` records, `$C4B990` workspace, `$C4E854` scratch words, and the
 * signed `$C45ACA` adjustment gate used by the alternative helper branches. */
int fa18_iterate_display_records(
    int16_t records[FA18_DISPLAY_RECORD_ITERATOR_RECORD_COUNT]
                   [FA18_DISPLAY_RECORD_ITERATOR_WORDS_PER_RECORD],
    int16_t workspace[FA18_DISPLAY_RECORD_ITERATOR_RECORD_COUNT][2],
    int16_t scratch[8], int16_t adjustment_gate_5aca);

#endif
