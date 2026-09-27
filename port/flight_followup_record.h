#ifndef FA18_FLIGHT_FOLLOWUP_RECORD_H
#define FA18_FLIGHT_FOLLOWUP_RECORD_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t auxiliary_flag;
    uint8_t selector_byte;
    uint8_t record_byte;
    uint16_t record_value;
    int32_t work_long;
    uint16_t record_index;
    int16_t indexed_word;
    int16_t scaled_word;
} FA18FlightFollowupRecordState;

typedef enum {
    FA18_FLIGHT_FOLLOWUP_EMPTY_CONTINUATION,
    FA18_FLIGHT_FOLLOWUP_COMPONENT_CALCULATION
} FA18FlightFollowupRecordRoute;

/* `$C1CCBC-$C1CD0D`: reset follow-up state and select table entry zero. */
int fa18_prepare_flight_followup_record(const uint8_t *record_table,
                                        size_t record_table_size,
                                        FA18FlightFollowupRecordState *state,
                                        FA18FlightFollowupRecordRoute *route,
                                        uint16_t *record_offset);

#endif
