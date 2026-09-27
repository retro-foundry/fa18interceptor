#include "flight_followup_record.h"

static uint16_t read_be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

int fa18_prepare_flight_followup_record(const uint8_t *record_table,
                                        size_t record_table_size,
                                        FA18FlightFollowupRecordState *state,
                                        FA18FlightFollowupRecordRoute *route,
                                        uint16_t *record_offset) {
    int16_t indexed_word;
    if (!record_table || record_table_size < 2 || !state || !route || !record_offset)
        return -1;
    *state = (FA18FlightFollowupRecordState){1, 4, 0, 0, 0, 0, 0, 0};
    indexed_word = (int16_t)read_be16(record_table);
    if (indexed_word <= 0) {
        *route = FA18_FLIGHT_FOLLOWUP_EMPTY_CONTINUATION;
        *record_offset = 0;
        return 0;
    }
    state->indexed_word = indexed_word;
    state->scaled_word = (int16_t)((uint16_t)indexed_word << 9);
    *record_offset = (uint16_t)state->scaled_word;
    *route = FA18_FLIGHT_FOLLOWUP_COMPONENT_CALCULATION;
    return 0;
}
