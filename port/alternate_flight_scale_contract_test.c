#include "alternate_flight_scale.h"
#include <assert.h>

static int alternate(void *context, uint16_t header, const int16_t tuple[3],
                     int32_t component[3]) {
    (void)context; assert(header == 0x1542 && tuple[0] == 2);
    component[0] = -256; component[1] = 511; component[2] = 0x123400;
    return 0;
}
int main(void) {
    FA18AlternateFlightScaleInput input = {0x1502, {2, -3, 4}, 0, 0, 0, alternate, 0};
    FA18AlternateFlightScaleState state = {0}; FA18AlternateFlightScaleRoute route;
    assert(fa18_scale_alternate_flight_record(&input, &state, &route) == 0);
    assert(route == FA18_ALTERNATE_FLIGHT_SCALE_VALUE_GATE && !state.component_ready &&
           state.component_word[0] == 2 && state.component_long[1] == -768);
    input.header = 0x1542;
    assert(fa18_scale_alternate_flight_record(&input, &state, &route) == 0);
    assert(route == FA18_ALTERNATE_FLIGHT_SCALE_FIXED_POINT && state.component_ready &&
           state.component_word[0] == -1 && state.component_word[1] == 1 &&
           state.component_word[2] == 0x1234);
    return 0;
}
