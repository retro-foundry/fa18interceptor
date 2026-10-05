#include "map_packet_depth_stage.h"

static int32_t negate_long(int32_t value) {
    return (int32_t)(UINT32_C(0) - (uint32_t)value);
}

static int32_t asr_long(int32_t value, unsigned shift) {
    if (value >= 0) return value >> shift;
    return (int32_t)-((-(int64_t)value + ((INT64_C(1) << shift) - 1)) >> shift);
}

int fa18_prepare_map_packet_depth_stage(
    const FA18MapPacketDepthStageInput *input,
    FA18MapPacketDepthStageResult *result) {
    int32_t metric;
    uint16_t divisor;
    if (!input || !input->packet || !result) return -1;

    result->renderer_words[0] = input->initial_bounds_ready ? 0x000f : 2;
    result->renderer_words[1] = input->initial_bounds_ready ? -1 : 2;
    result->renderer_words[2] = 0;
    result->renderer_words[3] = 0;
    metric = negate_long(input->packet->depth_metric);
    if (!input->metric_scale_inhibit && metric <= INT32_C(0x7fff0)) {
        metric = asr_long(metric, 4);
        divisor = (int16_t)input->metric_scale < 2 ? 2u : input->metric_scale;
        divisor = (uint16_t)(UINT16_C(0x8000) / divisor);
        metric = (int32_t)((uint32_t)(uint16_t)metric * divisor);
        metric = asr_long(metric, 4);
    }
    result->metric = metric;
    result->run_normal_pass = metric > INT32_C(0x3f8);
    result->run_wide_pass = 1;
    return 0;
}
