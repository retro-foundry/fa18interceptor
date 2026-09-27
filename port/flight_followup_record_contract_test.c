#include "flight_followup_record.h"

#include <assert.h>

int main(void) {
    uint8_t table[] = {0, 2};
    FA18FlightFollowupRecordState state;
    FA18FlightFollowupRecordRoute route;
    uint16_t offset;

    assert(fa18_prepare_flight_followup_record(table, sizeof table, &state, &route,
                                                &offset) == 0);
    assert(route == FA18_FLIGHT_FOLLOWUP_COMPONENT_CALCULATION &&
           state.auxiliary_flag == 1 && state.selector_byte == 4 &&
           !state.record_byte && !state.record_value && !state.work_long &&
           !state.record_index && state.indexed_word == 2 && state.scaled_word == 1024 &&
           offset == 1024);
    table[0] = 0; table[1] = 0;
    assert(fa18_prepare_flight_followup_record(table, sizeof table, &state, &route,
                                                &offset) == 0 &&
           route == FA18_FLIGHT_FOLLOWUP_EMPTY_CONTINUATION && !offset &&
           !state.indexed_word && !state.scaled_word);
    table[0] = 0xff; table[1] = 0xff;
    assert(fa18_prepare_flight_followup_record(table, sizeof table, &state, &route,
                                                &offset) == 0 &&
           route == FA18_FLIGHT_FOLLOWUP_EMPTY_CONTINUATION);
    assert(fa18_prepare_flight_followup_record(table, 1, &state, &route, &offset) == -1);
    return 0;
}
