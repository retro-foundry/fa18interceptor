#include "map_detail_gate.h"

#include <assert.h>

int main(void) {
    FA18MapDetailGateInput input = {0, 1, 0, 0x5a, 0x400};
    FA18MapDetailGateResult result;
    FA18MapDetailGateRoute route;
    assert(fa18_select_map_detail_gate(4, &input, &result, &route) == 0);
    assert(route == FA18_MAP_DETAIL_GATE_READY && result.detail_byte == 0x5a &&
           result.coordinate_shift == 2 && !result.visibility_flag);
    input.metric = 0x401;
    assert(fa18_select_map_detail_gate(4, &input, &result, &route) == 0);
    assert(result.coordinate_shift == 1);
    input.metric = 0xc81;
    assert(fa18_select_map_detail_gate(4, &input, &result, &route) == 0);
    assert(!result.coordinate_shift && result.detail_byte == 0x5a);
    input.metric = 0xc80;
    assert(fa18_select_map_detail_gate(3, &input, &result, &route) == 0);
    assert(result.coordinate_shift == 1 && result.visibility_flag == 1 && !result.detail_byte);
    input.alternate_layout = 0;
    assert(fa18_select_map_detail_gate((int8_t)0x83, &input, &result, &route) == 0);
    assert(result.mode == 3 && result.frame_flag == 1 && !result.coordinate_shift);
    assert(fa18_select_map_detail_gate(-1, &input, &result, &route) == 0 &&
           route == FA18_MAP_DETAIL_GATE_TERMINATOR);
    input.frame_flag = 1;
    assert(fa18_select_map_detail_gate(3, &input, &result, &route) == 0 &&
           route == FA18_MAP_DETAIL_GATE_FRAME_STOP);
    return 0;
}
