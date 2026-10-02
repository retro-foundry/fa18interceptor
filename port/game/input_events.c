#include "input_events.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const InputEventHooks *h, enum InputEventPhase phase,
                    uint32_t value, gaddr address) {
    if (h->observe) h->observe(h->context, phase, value, address);
}
static uint32_t consume(const InputEventHooks *h, enum InputEventChild child) {
    if (!h->consume) abort();
    return h->consume(h->context, child);
}
static void argument(const InputEventHooks *h, gaddr slot) {
    observe(h, INPUT_ARGUMENT, rd_u32(slot), 0);
}

/* C16EAE: absent events clear the latch; present events consume their word,
 * release the descriptor, then both paths refresh the directional input. */
void consume_external_input_event(const InputEventHooks *h) {
    uint32_t result;
    observe(h, INPUT_FRAME_BEGIN, 6, 0);
    argument(h, EXTERNAL_INPUT_HANDLE);
    result=consume(h, INPUT_EXTERNAL_READ);
    observe(h, INPUT_ARGUMENT_DROP, 0, 0);
    observe(h, INPUT_SOURCE_RESULT, result, 0);
    if (!result) {
        wr_u8(EXTERNAL_INPUT_LATCH, 0);
        observe(h, INPUT_EXTERNAL_EMPTY, 0, 0);
    } else {
        gaddr descriptor=rd_u32(EXTERNAL_INPUT_DESCRIPTOR);
        uint16_t code=rd_u16(descriptor+6);
        observe(h, INPUT_CODE_LOAD, code, descriptor);
        observe(h, INPUT_CODE_SAVE, code, 0);
        observe(h, INPUT_CODE_TEST, code, 0);
        if (code) {
            observe(h, INPUT_CODE_COMPARE_68, code, 0);
            if (code==0x68) consume(h, INPUT_CODE_68);
            else {
                observe(h, INPUT_CODE_COMPARE_E8, code, 0);
                if (code==0xe8) consume(h, INPUT_CODE_E8);
            }
        }
        /* The dispatcher may replace the descriptor. Reload before clearing. */
        descriptor=rd_u32(EXTERNAL_INPUT_DESCRIPTOR);
        wr_u16(descriptor+6, 0);
        observe(h, INPUT_CODE_CLEAR, 0, descriptor);
        argument(h, EXTERNAL_INPUT_RELEASE_HANDLE);
        consume(h, INPUT_EXTERNAL_RELEASE);
        observe(h, INPUT_ARGUMENT_DROP, 0, 0);
    }
    consume(h, INPUT_DIRECTION_REFRESH);
    observe(h, INPUT_FRAME_END, 0, 0);
}

/* C16BF2: the low byte is restored after release; the CPU adapter preserves
 * the other bytes produced by that child rather than returning a clean long. */
uint8_t read_keyboard_event_source(const InputEventHooks *h) {
    uint8_t key=0;
    uint32_t result;
    observe(h, INPUT_FRAME_BEGIN, 6, 0);
    argument(h, KEYBOARD_INPUT_HANDLE);
    result=consume(h, INPUT_KEYBOARD_READ);
    observe(h, INPUT_ARGUMENT_DROP, 0, 0);
    observe(h, INPUT_SOURCE_RESULT, result, 0);
    if (!result) observe(h, INPUT_KEY_EMPTY, 0, 0);
    else {
        gaddr descriptor=rd_u32(KEYBOARD_INPUT_DESCRIPTOR);
        uint16_t code=rd_u16(descriptor+6);
        key=(uint8_t)code;
        observe(h, INPUT_CODE_LOAD, code, descriptor);
        wr_u16(descriptor+6, 0);
        observe(h, INPUT_CODE_CLEAR, 0, descriptor);
        argument(h, KEYBOARD_RELEASE_HANDLE);
        observe(h, INPUT_KEY_SAVE, key, 0);
        consume(h, INPUT_KEYBOARD_RELEASE);
        observe(h, INPUT_ARGUMENT_DROP, 0, 0);
        observe(h, INPUT_KEY_RESTORE, key, 0);
    }
    observe(h, INPUT_FRAME_END, 0, 0);
    return key;
}

/* C16C56: a latched word bypasses decoding. Empty/filtered input returns
 * positive $00FF, while accepted release bytes are sign extended to a word. */
uint16_t poll_raw_keyboard_event(const InputEventHooks *h) {
    uint16_t result, latch=rd_u16(RAW_KEY_LATCH);
    observe(h, INPUT_FRAME_BEGIN, 4, 0);
    observe(h, INPUT_LATCH_TEST, latch, 0);
    if (latch) {
        wr_u16(RAW_KEY_LATCH, 0);
        observe(h, INPUT_LATCH_CLEAR, 0, 0);
        result=rd_u16(RAW_KEY_WORD);
        observe(h, INPUT_LATCH_VALUE, result, 0);
    } else {
        uint8_t raw=(uint8_t)consume(h, INPUT_RAW_SOURCE);
        observe(h, INPUT_RAW_SAVE, raw, 0);
        if (!raw) {
            result=0xff;
            observe(h, INPUT_RAW_EMPTY, result, 0);
        } else {
            uint8_t base=raw&0x7f;
            observe(h, INPUT_RAW_BASE, base, 0);
            observe(h, INPUT_RAW_FILTER, base&0x70, 0);
            if ((base&0x70)==0x70) {
                result=0xff;
                observe(h, INPUT_RAW_EMPTY, result, 0);
            } else {
                observe(h, INPUT_RAW_PRESS, raw&0x80, 0);
                if (raw&0x80) {
                    observe(h, INPUT_RAW_RELEASE, base, 0);
                    base+=0x80;
                }
                result=(uint16_t)(int16_t)(int8_t)base;
                observe(h, INPUT_RAW_RETURN, base, 0);
            }
        }
    }
    observe(h, INPUT_FRAME_END, 0, 0);
    return result;
}

/* C13D34: both button bits consume the mirror only after both readiness
 * gates. Command $77 takes priority; signed values $78..$7F select $79. */
void consume_changed_buttons(const InputEventHooks *h) {
    uint32_t metric=rd_u32(MATRIX_SIDE_METRIC);
    uint16_t buttons, flags;
    uint8_t ready, level;
    observe(h, INPUT_BUTTON_METRIC, metric, 0);
    if (!metric) return;
    ready=rd_u8(PLAYER_READY);
    observe(h, INPUT_BUTTON_READY, ready, 0);
    if (!ready) return;
    buttons=rd_u16(INPUT_STATE_MIRROR);
    observe(h, INPUT_BUTTON_MASK, buttons, 0);
    if ((buttons&3)!=3) return;
    flags=rd_u16(BUTTON_COMMAND_FLAGS);
    observe(h, INPUT_BUTTON_FLAGS, flags, 0);
    if (flags&8) {
        wr_u8(FUNCTION_KEY_LEVEL, 0x77);
        observe(h, INPUT_BUTTON_COMMAND, 0x77, 0);
    } else {
        level=rd_u8(BUTTON_COMMAND_LEVEL);
        observe(h, INPUT_BUTTON_LEVEL, level, 0);
        if ((int8_t)level>=0x78) {
            wr_u8(FUNCTION_KEY_LEVEL, 0x79);
            observe(h, INPUT_BUTTON_COMMAND, 0x79, 0);
        }
    }
    wr_u16(INPUT_STATE_MIRROR, 0);
    observe(h, INPUT_BUTTON_CLEAR, 0, 0);
}
