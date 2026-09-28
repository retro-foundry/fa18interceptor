#ifndef FA18_DISPLAY_RECORD_ITERATOR_H
#define FA18_DISPLAY_RECORD_ITERATOR_H

#include <stdint.h>

#include "display_record_projection.h"

enum {
    FA18_DISPLAY_RECORD_ITERATOR_RECORD_COUNT = 8,
    FA18_DISPLAY_RECORD_ITERATOR_WORDS_PER_RECORD = 8
};

/* `$C2E758-$C2EC67`: the caller supplies the already-selected, 16-byte-stride
 * `$C4B390` records and `$C4E854` scratch words. */
int fa18_iterate_display_records(
    int16_t records[FA18_DISPLAY_RECORD_ITERATOR_RECORD_COUNT]
                   [FA18_DISPLAY_RECORD_ITERATOR_WORDS_PER_RECORD],
    int16_t workspace[FA18_DISPLAY_RECORD_ITERATOR_RECORD_COUNT][2],
    int16_t scratch[8], int16_t case_signed_word);

#endif
