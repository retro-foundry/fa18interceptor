#ifndef FA18_GAME_INDEXED_RECORD_UPDATE_H
#define FA18_GAME_INDEXED_RECORD_UPDATE_H

#include "memory.h"

/* State carried between the source blocks of $C13D84. */
typedef struct IndexedRecordWork {
    gaddr record;
    int16_t index;
    int16_t neg_word_6c;
    int16_t reference_distance;
    int16_t phase_byte_scaled;
    int16_t current_word_6c;
    int16_t attitude_adjustment;
    int16_t comparison_bound;
    gaddr helper_record;
    int16_t helper_m6c, helper_m6e, helper_old76;
    int helper_enabled;
} IndexedRecordWork;

void prepare_indexed_record_context(IndexedRecordWork *work);

/* $C1414E-$C142A5. Selects the signed response route. */
typedef enum IndexedSignedRoute {
    INDEXED_COMMON_TAIL,
    INDEXED_EMPTY_ROUTE,
    INDEXED_OTHER_ROUTE
} IndexedSignedRoute;
IndexedSignedRoute prepare_indexed_signed_terms(IndexedRecordWork *work);

/* $C14600-$C146C1: the path with no active $72 longword or phase/flag gate. */
void settle_empty_indexed_record(IndexedRecordWork *work);

/* $C142A6-$C145FC: signed phase and header-bit-7 arithmetic routes. */
void update_indexed_other_route(IndexedRecordWork *work);

/* $C146C2-$C14874: common flags, damping, child update and +$6E result. */
void finish_indexed_record_update(IndexedRecordWork *work);

/* The complete memory-side work of $C13D84, before register glue. */
void update_indexed_record(IndexedRecordWork *work);

#endif
