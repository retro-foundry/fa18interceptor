#ifndef FA18_GAME_INDEXED_RECORD_UPDATE_H
#define FA18_GAME_INDEXED_RECORD_UPDATE_H

#include "memory.h"

/* State carried from $C13D84-$C1414E into the remaining record update. */
typedef struct IndexedRecordWork {
    gaddr record;
    int16_t index;
    int16_t neg_word_6c;
    int16_t reference_distance;
    int16_t phase_byte_scaled;
} IndexedRecordWork;

void prepare_indexed_record_context(IndexedRecordWork *work);

#endif
