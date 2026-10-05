/* Source: $C1BC50-$C1BEE4, plus $C1C214's shared byte toggle.
 * Reference mapping and CPU oracle: analysis/routines/native_indexed_controls.md.
 * This is the ordinary-state counterpart of game/indexed_commands.c. */
#include "indexed_controls.h"

int fa18_apply_indexed_control(FA18IndexedControls *s,
                              const FA18IndexedControlRequest *r,
                              int16_t index,
                              const FA18IndexedControlPoses *poses,
                              FA18IndexedStatusTone status_tone,
                              void *context, uint32_t *published_event) {
    uint32_t event;
    uint8_t level;
    if (!s || !r || !published_event || !status_tone ||
        r->kind < FA18_INDEXED_FUNCTION_KEY || r->kind > FA18_INDEXED_SELECTION)
        return 0;
    event = r->event;
    if (r->kind == FA18_INDEXED_FUNCTION_KEY) {
        if ((int16_t)event > 0x59 || (s->mode && !s->function_modifier))
            goto throttle_level;
        index = (int16_t)((uint16_t)event - 0x50u + 10u);
    } else if (r->kind == FA18_INDEXED_LOW_KEY) {
        index = (int16_t)((uint16_t)event - 1u);
    } else {
        index = r->selection;
    }
    if (s->enable_gate) {
        if (index >= 0 && index <= 1) {
            s->enable_selection = index == 1 ? 0x10 : 0x11;
            s->enable_gate = 0;
        }
        goto publish;
    }
    if (!s->mode_gate) {
        if (index >= 0 && index <= 8) s->mode_gate = (uint8_t)(index + 1);
        goto publish;
    }
    if (s->mode) goto select_pose;
    if ((int8_t)index < 0) goto publish;
    switch ((uint8_t)index) {
    case 0:
        s->mode_request = 1;
        if (!s->playback_bytes) goto publish;
        index = 0x7f;
        goto store_mode;
    case 1:
        if (r->modifier) s->mode_request = 2;
        goto store_mode;
    case 2: goto store_mode;
    case 3: index = 0x7d; goto store_mode;
    case 4: index = 9; goto store_mode;
    case 5: index = -1; goto store_mode;
    case 7: index = -2; goto store_mode;
    default: break;
    }
    if (!s->modes.status) {
        event = status_tone(context, s);
        goto publish;
    }
    if ((uint8_t)index == 6) {
        s->mode_request = 1;
        level = (uint8_t)(s->modes.level + 1);
        index = (int16_t)(((uint16_t)index & 0xff00u) | level);
        if ((int8_t)level < 3 || (int8_t)level > 8) index = 3;
        goto store_mode;
    }
    level = (uint8_t)((uint8_t)index - 10);
    if ((int8_t)level < 0 || (int8_t)level > 5) goto publish;
    level = (uint8_t)(level + 3);
    index = (int16_t)(((uint16_t)index & 0xff00u) | level);
    if (level == 3) goto store_mode;
    if (!s->modes.available[level - 1]) goto publish;
store_mode:
    s->mode = (uint8_t)index;
    event = status_tone(context, s);
    goto publish;

select_pose:
    if (s->cockpit_high_byte & 8) {
        /* $C1BEDE only increments a dead carried word; $C1BEE4 branches
         * straight to publication. It does not select or write a pose. */
        goto publish;
    }
    if (!poses || !poses->values || !poses->count) return 0;
    {
        size_t entry;
        int16_t offset = (int16_t)((int16_t)(int8_t)index * 16);
        for (entry = 0; entry < poses->count; ++entry) {
            int16_t pose = poses->values[entry];
            if (pose < 0) {
                if (pose == -1) goto publish;
                break;
            }
            if (offset <= (int16_t)(entry * 16)) break;
        }
        if (entry == poses->count) return 0;
    }
    s->pose_entry = (uint8_t)index;
    if ((int8_t)s->origin_detail >= 0 && !s->pose_inhibit)
        s->origin_gate_b = s->origin_gate_b ? 0 : 1;
    goto publish;

throttle_level:
    if ((int8_t)s->recorder_mode > 0 || s->recorder_mode || s->origin_gate_a)
        goto publish;
    /* $C1BCEE's recorder $FD path changes only the carried word and returns
     * via $C1BEDA. No typed game state or published event changes there. */
    index = (int16_t)((uint16_t)event - 0x4fu);
    if (index == 1 && ((!s->function_level && s->control_record_level == 0x0c) ||
                       s->function_level == 0x0c)) {
        s->function_level = 0xff;
        goto publish;
    }
    level = (uint8_t)((uint8_t)index * 12);
    if ((int8_t)level >= 0x78) ++level;
    s->function_level = level;
    if (s->player_ready) {
        index = (int16_t)((int16_t)(int8_t)level * 8);
        if (index > 0x3c0) index = 0x3c0;
        s->throttle = index;
        s->throttle_companion = index;
    }
publish:
    *published_event = event;
    return 1;
}
