#ifndef FA18_CONTEXT_REFRESH_PACKET_H
#define FA18_CONTEXT_REFRESH_PACKET_H

#include <stddef.h>
#include <stdint.h>

typedef int (*FA18ContextRefreshStage)(void *context);
typedef int (*FA18ContextRefreshError)(void *context, uint16_t code);

typedef struct {
    int32_t guard_long;
    uint8_t request_bits;
    uint8_t prepared_flag;
    uint8_t callback_a_flag;
    uint8_t callback_b_flag;
    uint8_t context_selection;
    const uint8_t *active_record;
    size_t active_record_size;
    int32_t origin[3];
    int16_t selector_x;
    int16_t selector_z;
    uint16_t trace_word;
    uint16_t error_word;
    uint16_t stage_c_selector;
    uint8_t render_guard_a;
    uint8_t render_guard_b;
    uint32_t render_flag;
    uint16_t render_selector;
    uint8_t frame_local_enable;
} FA18ContextRefreshPacketState;

typedef struct {
    FA18ContextRefreshStage flag_records;
    FA18ContextRefreshError report_error;
    FA18ContextRefreshStage scene_selector;
    FA18ContextRefreshStage stage_a;
    FA18ContextRefreshStage stage_b;
    FA18ContextRefreshStage stage_c_clear;
    FA18ContextRefreshStage stage_c_set;
    FA18ContextRefreshStage submit_render;
    void *context;
} FA18ContextRefreshPacketOps;

/* `$C1C860-$C1CA2D`: refresh selector terms, consume pending request bits,
 * run the ordered context callbacks, and conditionally submit the renderer
 * packet. Each named child remains caller-owned. */
int fa18_run_context_refresh_packet(FA18ContextRefreshPacketState *state,
                                    const FA18ContextRefreshPacketOps *ops);

#endif
