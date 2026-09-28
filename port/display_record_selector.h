#ifndef FA18_DISPLAY_RECORD_SELECTOR_H
#define FA18_DISPLAY_RECORD_SELECTOR_H

#include <stdint.h>

enum { FA18_DISPLAY_RECORD_SELECTOR_WORDS = 16 };

typedef struct {
    uint8_t mode_flag;
    uint16_t sequence_flags;
    int16_t threshold_source;
    int16_t mode_zero_threshold_source;
    int16_t mode_nonzero_threshold_source;
} FA18DisplayRecordSelectorInput;

typedef struct {
    int16_t words[FA18_DISPLAY_RECORD_SELECTOR_WORDS];
    uint16_t selection_word_a;
    uint16_t selection_word_b;
    uint32_t selection_long;
    uint8_t selection_flag;
} FA18DisplayRecordSelectorOutput;

/* `$C0D7E0-$C0DA9F`: selects workspace pairs and writes the source's
 * `$C4B390` record layout. Returns zero for `$C0DA70` success, one for the
 * `$C0DA94` rejection, and minus one for invalid native inputs. */
int fa18_select_display_record_pairs(const int16_t workspace[8][2],
                                     const int16_t scratch[8],
                                     const FA18DisplayRecordSelectorInput *input,
                                     FA18DisplayRecordSelectorOutput *output);

#endif
