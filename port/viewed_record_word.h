#ifndef FA18_NATIVE_VIEWED_RECORD_WORD_H
#define FA18_NATIVE_VIEWED_RECORD_WORD_H
#include "native_scene_records.h"
#include "startup_ranges.h"

typedef struct {
    FA18NativeSceneRecords *records;
    FA18FlightCommandState *flight;
    PortFieldWordValue value;
} FA18NativeViewedRecordWord;
/* Import the original bounded record-offset word into the actual native
 * viewed reference, then replace its startup field pair with a logical-word
 * accessor. Only resolved members of this supplied bank are accepted; an
 * unsupported offset/reference fails, never selecting a default record.
 * Rebinding the same owner rereads its live pointer. Keep the owner stable. */
int fa18_bind_native_viewed_record_word(FA18NativeViewedRecordWord *owner,
                                         FA18NativeSceneRecords *records,
                                         FA18FlightCommandState *flight,
                                         PortFieldByte *startup_words,size_t count);
#endif
