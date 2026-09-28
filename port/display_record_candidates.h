#ifndef FA18_DISPLAY_RECORD_CANDIDATES_H
#define FA18_DISPLAY_RECORD_CANDIDATES_H

#include <stdint.h>

#include "hunk.h"

enum {
    FA18_DISPLAY_RECORD_CANDIDATE_COUNT = 4,
    FA18_DISPLAY_RECORD_CANDIDATE_HUNK = 33,
    FA18_DISPLAY_RECORD_CANDIDATE_INPUT_OFFSET = 0,
    FA18_DISPLAY_RECORD_CANDIDATE_INPUT_BYTES = 16
};

typedef struct {
    int16_t x;
    int16_t y;
    int16_t depth;
} FA18DisplayRecordCandidate;

/* `$C0D75E` addresses its own segment-33 `$C0D720` prefix as four signed
 * input pairs.  The words are executable bytes in the source image, so they
 * must be loaded from the original hunk rather than embedded as a fixture. */
int fa18_load_display_record_candidate_input_pairs(
    const FA18Hunks *hunks,
    int16_t output[FA18_DISPLAY_RECORD_CANDIDATE_COUNT][2]);

/* `$C0D752-$C0D7D2`: forms the four `$1a`-stride `$C4B390` candidates
 * preceding the `$C2E758` iterator. `source_component` is the high word of
 * `$C45A66`; the source arithmetic-shifts it by two before every row. */
int fa18_prepare_display_record_candidates(
    const int16_t input_pairs[FA18_DISPLAY_RECORD_CANDIDATE_COUNT][2],
    int16_t source_component, const int16_t matrix[3][3],
    FA18DisplayRecordCandidate output[FA18_DISPLAY_RECORD_CANDIDATE_COUNT]);

#endif
