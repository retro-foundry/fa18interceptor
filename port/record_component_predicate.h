#ifndef FA18_RECORD_COMPONENT_PREDICATE_H
#define FA18_RECORD_COMPONENT_PREDICATE_H

#include <stdint.h>

/* `$C1FB82-$C1FC41`: the record-triple predicate used by the `$C200A8`
 * prepared-record dispatch.  A nonzero `$0C00` bit pair transfers to the
 * still-unrecovered `$C1FC3A` branch and is deliberately rejected here. */
typedef struct {
    int16_t workspace[9]; /* `$C4BF94`: p, q, r triples. */
    int16_t descriptor_word; /* source A3.w; its signed high-byte selects shift. */
    int16_t stream_stage_shift; /* `$C45AB8`. */
    int16_t record_component_x; /* `$C45B2A`. */
    int16_t record_component_z; /* `$C45B2E`. */
    int16_t local_component[3]; /* `-38(a6), -36(a6), -34(a6)`. */
    const int16_t *control_base;
    uint16_t control_word_count;
} FA18RecordComponentPredicateState;

typedef enum {
    FA18_RECORD_COMPONENT_PREDICATE_ACCEPTED,
    FA18_RECORD_COMPONENT_PREDICATE_REJECTED,
    FA18_RECORD_COMPONENT_PREDICATE_C1FC3A_EXTERNAL
} FA18RecordComponentPredicateRoute;

/* Returns the source-visible D7 result and next stream word index.  The
 * rejected result retains D7's high word and clears only its low word, exactly
 * as `$C1FC36` does. */
int fa18_test_record_component_predicate(
    const FA18RecordComponentPredicateState *state, uint32_t input_d7,
    uint16_t stream_word_index, uint32_t *result_d7,
    uint16_t *next_stream_word_index, FA18RecordComponentPredicateRoute *route);

#endif
