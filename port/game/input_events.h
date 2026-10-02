#ifndef FA18_INPUT_EVENTS_H
#define FA18_INPUT_EVENTS_H
#include "memory.h"

/* Complete C16EAE/C16BF2/C16C56/C13D34 input owners. External descriptor
 * ownership and physical key mappings remain in their original consumers. */
enum InputEventChild {
    INPUT_EXTERNAL_READ, INPUT_CODE_68, INPUT_CODE_E8, INPUT_EXTERNAL_RELEASE,
    INPUT_DIRECTION_REFRESH, INPUT_KEYBOARD_READ, INPUT_KEYBOARD_RELEASE,
    INPUT_RAW_SOURCE
};
enum InputEventPhase {
    INPUT_FRAME_BEGIN, INPUT_FRAME_END, INPUT_ARGUMENT, INPUT_ARGUMENT_DROP,
    INPUT_SOURCE_RESULT, INPUT_EXTERNAL_EMPTY, INPUT_CODE_LOAD, INPUT_CODE_SAVE,
    INPUT_CODE_TEST, INPUT_CODE_COMPARE_68, INPUT_CODE_COMPARE_E8,
    INPUT_CODE_CLEAR, INPUT_KEY_SAVE, INPUT_KEY_RESTORE, INPUT_KEY_EMPTY,
    INPUT_LATCH_TEST, INPUT_LATCH_CLEAR, INPUT_LATCH_VALUE, INPUT_RAW_SAVE,
    INPUT_RAW_EMPTY, INPUT_RAW_BASE, INPUT_RAW_FILTER, INPUT_RAW_PRESS,
    INPUT_RAW_RELEASE, INPUT_RAW_RETURN, INPUT_BUTTON_METRIC, INPUT_BUTTON_READY,
    INPUT_BUTTON_MASK, INPUT_BUTTON_FLAGS, INPUT_BUTTON_LEVEL,
    INPUT_BUTTON_COMMAND, INPUT_BUTTON_CLEAR
};
typedef struct {
    uint32_t (*consume)(void *context, enum InputEventChild child);
    void (*observe)(void *context, enum InputEventPhase phase, uint32_t value,
                    gaddr address);
    void *context;
} InputEventHooks;
void consume_external_input_event(const InputEventHooks *hooks);
uint8_t read_keyboard_event_source(const InputEventHooks *hooks);
uint16_t poll_raw_keyboard_event(const InputEventHooks *hooks);
void consume_changed_buttons(const InputEventHooks *hooks);
#endif
