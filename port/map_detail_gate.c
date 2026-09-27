#include "map_detail_gate.h"

int fa18_select_map_detail_gate(int8_t encoded_mode,
                                const FA18MapDetailGateInput *input,
                                FA18MapDetailGateResult *result,
                                FA18MapDetailGateRoute *route) {
    if (!input || !result || !route) return -1;
    if (input->frame_flag) {
        *route = FA18_MAP_DETAIL_GATE_FRAME_STOP;
        return 0;
    }
    if (encoded_mode == -1) {
        *route = FA18_MAP_DETAIL_GATE_TERMINATOR;
        return 0;
    }
    result->frame_flag = input->frame_flag;
    result->mode = (uint8_t)encoded_mode;
    if (encoded_mode < 0) {
        ++result->frame_flag;
        result->mode &= 0x7fu;
    }
    result->visibility_flag = 0;
    result->coordinate_shift = 0;
    result->detail_byte = 0;
    if (!input->alternate_layout || input->shared_gate) {
        *route = FA18_MAP_DETAIL_GATE_READY;
        return 0;
    }
    if (result->mode == 4) {
        result->detail_byte = input->shared_detail_byte;
        if (input->metric <= INT32_C(0x400)) result->coordinate_shift = 2;
        else if (input->metric <= INT32_C(0xc80)) result->coordinate_shift = 1;
    } else if (input->metric <= INT32_C(0xc80)) {
        result->coordinate_shift = 1;
        result->visibility_flag = 1;
    }
    *route = FA18_MAP_DETAIL_GATE_READY;
    return 0;
}
