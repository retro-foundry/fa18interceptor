#include "context_refresh_packet.h"

static int16_t read_be16(const uint8_t *bytes) {
    return (int16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}
static int16_t asr_word_2(int16_t value) {
    return value >= 0 ? (int16_t)(value >> 2) :
        (int16_t)-((-(int32_t)value + 3) >> 2);
}
static int call(FA18ContextRefreshStage stage, void *context) {
    return stage && stage(context) == 0 ? 0 : -1;
}

int fa18_run_context_refresh_packet(FA18ContextRefreshPacketState *state,
                                    const FA18ContextRefreshPacketOps *ops) {
    uint8_t requests;
    if (!state || !ops || !ops->stage_a || !ops->stage_b ||
        !ops->stage_c_clear || !ops->stage_c_set)
        return -1;
    if (state->guard_long < (int32_t)UINT32_C(0xf8000000)) return 0;
    state->frame_local_enable = 0;
    requests = state->request_bits;
    if (requests) {
        uint8_t class_value;
        if (!ops->classify || ops->classify(ops->context, &class_value) != 0)
            return -1;
        switch (class_value & 0x0fu) {
        case 0x0b: case 0x0c: case 0x0f: break;
        default:
            state->error_word = 0x27;
            if (!ops->report_error || ops->report_error(ops->context, 0x27) != 0)
                return -1;
            break;
        }
        if (state->context_selection) {
            state->selector_x = asr_word_2((int16_t)(((uint32_t)state->origin[0] &
                                                       UINT32_C(0x1fffffff)) >> 16));
            state->selector_z = asr_word_2((int16_t)(((uint32_t)state->origin[2] &
                                                       UINT32_C(0x1fffffff)) >> 16));
        } else {
            if (!state->active_record || state->active_record_size < 10) return -1;
            state->selector_x = asr_word_2(read_be16(state->active_record + 6));
            state->selector_z = asr_word_2(read_be16(state->active_record + 8));
        }
        state->prepared_flag = 1;
        state->callback_a_flag = 0;
        state->callback_b_flag = 0;
        if (requests & 1u) {
            state->request_bits &= (uint8_t)~1u;
            if (call(ops->scene_selector, ops->context) != 0) return -1;
        }
        if (requests & 2u) {
            state->request_bits &= (uint8_t)~2u;
            state->callback_b_flag = 1;
            if (call(ops->scene_selector, ops->context) != 0) return -1;
        }
        state->callback_a_flag = 1;
        if (requests & 4u) state->request_bits &= (uint8_t)~4u;
        if (requests & 8u) {
            state->request_bits &= (uint8_t)~8u;
            if (call(ops->scene_selector, ops->context) != 0) return -1;
            state->trace_word = 0x4a;
        }
        state->frame_local_enable = 1;
        state->request_bits = 0;
    }
    if (call(ops->stage_a, ops->context) != 0) return -1;
    state->trace_word = 0x4b;
    state->frame_local_enable = 0;
    if (call(ops->stage_b, ops->context) != 0) return -1;
    state->trace_word = 0x4c;
    if (call((state->stage_c_selector & 1u) ? ops->stage_c_set : ops->stage_c_clear,
             ops->context) != 0)
        return -1;
    state->trace_word = (state->stage_c_selector & 1u) ? 0x4e : 0x4d;
    if (!state->render_guard_a && state->render_guard_b) {
        state->render_selector = state->prepared_flag ? 0x0f : 6;
        if (state->prepared_flag) state->render_flag = 0x000fffff;
        if (!ops->submit_render || ops->submit_render(ops->context) != 0) return -1;
    }
    state->prepared_flag = 0;
    return 0;
}
