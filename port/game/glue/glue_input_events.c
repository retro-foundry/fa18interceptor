/* Whole-call CPU adapters, independent of the input timing bridge. */
#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "input_events.h"

static const struct { uint32_t entry, ret; } children[]={
    {0xc53c08,0xc16ebe}, {0xc0833e,0xc16eee}, {0xc08394,0xc16efe},
    {0xc53c8c,0xc16f14}, {0xc16f1c,0xc16f18},
    {0xc53c08,0xc16c02}, {0xc53c8c,0xc16c30}, {0xc16bf2,0xc16c76}
};
static uint32_t consume(void *context, enum InputEventChild child) {
    (void)context;
    return (uint32_t)glue_complete_child(children[child].entry, children[child].ret);
}
static void outputs(void *context, enum InputEventPhase phase, uint32_t value,
                    gaddr address) {
    (void)context;
    switch (phase) {
    case INPUT_FRAME_BEGIN: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=value; break;
    case INPUT_FRAME_END: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    case INPUT_ARGUMENT: m68ki_push_32(value); flags_logic_l(value); break;
    case INPUT_ARGUMENT_DROP: A(7)+=4; break;
    case INPUT_SOURCE_RESULT: wr_u32(A(6)-6,value); flags_logic_l(value); break;
    case INPUT_EXTERNAL_EMPTY: flags_logic_b(0); break;
    case INPUT_CODE_LOAD: A(0)=address; SET_W(D(0),value); flags_logic_w(value); break;
    case INPUT_CODE_SAVE: wr_u16(A(6)-2,(uint16_t)value); flags_logic_w(value); break;
    case INPUT_CODE_TEST: flags_logic_w(value); break;
    case INPUT_CODE_COMPARE_68: step_compare_word(0x68,value); break;
    case INPUT_CODE_COMPARE_E8: step_compare_word(0xe8,value); break;
    case INPUT_CODE_CLEAR: A(0)=address; flags_logic_w(0); break;
    case INPUT_KEY_SAVE: wr_u8(A(6)-1,(uint8_t)value); flags_logic_b(value); break;
    case INPUT_KEY_RESTORE: SET_B(D(0),value); flags_logic_b(value); break;
    case INPUT_KEY_EMPTY: D(0)=0; flags_logic_l(0); break;
    case INPUT_LATCH_TEST: case INPUT_LATCH_CLEAR: flags_logic_w(value); break;
    case INPUT_LATCH_VALUE: case INPUT_RAW_EMPTY: SET_W(D(0),value); flags_logic_w(value); break;
    case INPUT_RAW_SAVE: wr_u8(A(6)-2,(uint8_t)value); flags_logic_b(value); break;
    case INPUT_RAW_BASE: SET_B(D(0),value); wr_u8(A(6)-1,(uint8_t)value); flags_logic_b(value); break;
    case INPUT_RAW_FILTER: SET_B(D(0),value); step_compare_byte(0x70,value); break;
    case INPUT_RAW_PRESS:
        SET_B(D(0),value); flags_logic_b(value);
        SET_B(D(1),value?0:0xff); renderer_negate(&D(1),1);
        SET_W(D(1),(int16_t)(int8_t)D(1));
        D(1)=(uint32_t)(int32_t)(int16_t)D(1);
        wr_u16(A(6)-4,(uint16_t)D(1)); flags_logic_w(D(1)); break;
    case INPUT_RAW_RELEASE:
        SET_B(D(0),value); SET_W(D(0),(int16_t)(int8_t)D(0));
        D(0)=(uint32_t)(int32_t)(int16_t)D(0); step_add_long(&D(0),0x80);
        wr_u8(A(6)-1,(uint8_t)D(0)); flags_logic_b(D(0)); break;
    case INPUT_RAW_RETURN: SET_B(D(0),value); SET_W(D(0),(int16_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case INPUT_BUTTON_METRIC: flags_logic_l(value); break;
    case INPUT_BUTTON_READY: flags_logic_b(value); break;
    case INPUT_BUTTON_MASK: SET_W(D(0),value&3); step_subtract_word(&D(0),3); break;
    case INPUT_BUTTON_FLAGS: SET_W(D(0),value); flags_logic_w(value); FLAG_Z=value&8; break;
    case INPUT_BUTTON_LEVEL: SET_B(D(0),value); step_compare_byte(0x78,value); break;
    case INPUT_BUTTON_COMMAND: flags_logic_b(value); break;
    case INPUT_BUTTON_CLEAR: flags_logic_w(0); break;
    }
}
static const InputEventHooks hooks={consume, outputs, NULL};
int glue_C16EAE(void) { consume_external_input_event(&hooks); return glue_return(); }
int glue_C16BF2(void) { read_keyboard_event_source(&hooks); return glue_return(); }
int glue_C16C56(void) { poll_raw_keyboard_event(&hooks); return glue_return(); }
int glue_C13D34(void) { consume_changed_buttons(&hooks); return glue_return(); }
