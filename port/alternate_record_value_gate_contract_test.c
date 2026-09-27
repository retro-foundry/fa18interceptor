#include "alternate_record_value_gate.h"
#include <assert.h>

static int fixed(void *context, const int16_t component[3], int16_t *value) {
    unsigned *calls = context; assert(component[0] == 1); ++*calls; *value = 77; return 0;
}
int main(void) {
    unsigned calls = 0;
    FA18AlternateRecordValueGateInput input = {1, 0, 0x80, {1, 2, 3}, {256, 512, 768}, fixed, &calls};
    FA18AlternateRecordValueGateState state;
    assert(fa18_gate_alternate_record_value(&input, &state) == 0 &&
           state.fixed_point_called && state.record_value == 77 && calls == 1 &&
           state.component_long[2] == 768);
    input.record_value = 0x100; input.selector_control = 1;
    assert(fa18_gate_alternate_record_value(&input, &state) == 0 && !state.fixed_point_called);
    input.header = 0x101; input.selector_control = 2;
    assert(fa18_gate_alternate_record_value(&input, &state) == 0 && state.fixed_point_called);
    input.record_value = 0x400;
    assert(fa18_gate_alternate_record_value(&input, &state) == 0 && !state.fixed_point_called);
    return 0;
}
