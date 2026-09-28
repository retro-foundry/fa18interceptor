#ifndef FA18_PREPARED_RECORD_DISPATCH_H
#define FA18_PREPARED_RECORD_DISPATCH_H

#include <stddef.h>
#include <stdint.h>

#include "record_component_predicate.h"

/* `$C2005C-$C200F5`: two direct vertex offsets, then ordinary offsets until a
 * negative terminator whose low fifteen bits select the final vertex.  The
 * following descriptor word is consumed by `$C200AE`, not as a vertex. */
typedef struct {
    const int16_t *vertex_words; /* `$C48390`, six bytes per transformed tuple. */
    size_t vertex_word_count;
    const int16_t *stream_words; /* caller-owned `(A2)+` stream */
    size_t stream_word_count;
    uint16_t stream_word_index;
    FA18RecordComponentPredicateState predicate;
    int16_t *workspace_words; /* `$C4BF94` onward, supplied by the owner */
    size_t workspace_word_capacity;
} FA18PreparedRecordDispatchInput;

typedef enum {
    FA18_PREPARED_RECORD_DISPLAY_READY,
    FA18_PREPARED_RECORD_NEGATIVE_STATUS,
    FA18_PREPARED_RECORD_C1FC3A_EXTERNAL
} FA18PreparedRecordDispatchRoute;

typedef struct {
    uint16_t next_stream_word_index;
    uint16_t triple_count; /* `$C4BF92` */
    int16_t selected_record_word; /* `$C45954`, for `$C2469E` */
    uint16_t component_test_count_delta; /* `-50(A6)` */
    uint16_t component_reject_count_delta; /* `-52(A6)` */
    FA18PreparedRecordDispatchRoute route;
} FA18PreparedRecordDispatchResult;

/* Stops immediately before `$C2469E`; the caller owns its clipping/page state. */
int fa18_dispatch_prepared_record_triples(
    const FA18PreparedRecordDispatchInput *input,
    FA18PreparedRecordDispatchResult *result);

#endif
