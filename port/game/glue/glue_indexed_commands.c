#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_indexed_commands.h"

static uint32_t mode_changed(void *context) {
    (void)context; return (uint32_t)glue_complete_child(0xc3318e,0xc1bdf8);
}
static void outputs(void *context,enum IndexedCommandPhase phase,uint32_t value,
                    uint32_t limit,gaddr address) {
    (void)context;
    switch(phase) {
    case INDEXED_BYTE_TEST: case INDEXED_BYTE_STORE: case INDEXED_MODIFIER_TEST:
        flags_logic_b(value); break;
    case INDEXED_WORD_TEST: case INDEXED_WORD_STORE: flags_logic_w(value); break;
    case INDEXED_LONG_TEST: flags_logic_l(value); break;
    case INDEXED_EVENT_COMPARE: step_compare_word(limit,D(0)); break;
    case INDEXED_EVENT_COPY: SET_W(D(4),D(0)); flags_logic_w(D(4)); break;
    case INDEXED_SUBTRACT_WORD:
        if(address) step_subtract_byte(&D(4),limit);
        else step_subtract_word(&D(4),limit); break;
    case INDEXED_ADD_WORD: step_add_word(&D(4),limit); break;
    case INDEXED_COMPARE_WORD: step_compare_word(limit,D(4)); break;
    case INDEXED_COMPARE_BYTE: step_compare_byte(limit,value); break;
    case INDEXED_SELECT_BYTE: case INDEXED_TABLE_LEVEL_READ:
        SET_B(D(4),value); flags_logic_b(value); break;
    case INDEXED_SELECT_CONSTANT: D(4)=value; flags_logic_l(value); break;
    case INDEXED_RECORDER_READ: SET_B(D(2),value); flags_logic_b(value); break;
    case INDEXED_RECORDER_COMPARE: step_compare_byte(limit,D(2)); break;
    case INDEXED_RECORDER_TEST: flags_logic_b(D(2)); break;
    case INDEXED_LEVEL_DOUBLE: renderer_add_byte(&D(4),D(4)); break;
    case INDEXED_LEVEL_SAVE: SET_B(D(5),D(4)); flags_logic_b(D(5)); break;
    case INDEXED_LEVEL_ADD: renderer_add_byte(&D(4),D(5)); break;
    case INDEXED_LEVEL_INCREMENT: renderer_add_byte(&D(4),limit); break;
    case INDEXED_LEVEL_EXTEND: SET_W(D(4),(int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case INDEXED_LEVEL_SCALE: renderer_asl_word(&D(4),3); break;
    case INDEXED_LEVEL_COMPARE: step_compare_word(limit,D(4)); break;
    case INDEXED_LEVEL_CLAMP: SET_W(D(4),value); flags_logic_w(value); break;
    case INDEXED_MODE_TABLE: A(0)=address; break;
    case INDEXED_TABLE_LEVEL_INCREMENT: renderer_add_byte(&D(4),1); break;
    case INDEXED_TABLE_AVAILABILITY:
        A(0)=address; SET_W(D(4),(int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case INDEXED_POSE_TEST: FLAG_Z=value&8u; break;
    case INDEXED_POSE_BEGIN:
        A(0)=address; SET_B(D(1),D(4)); SET_W(D(1),(int16_t)(int8_t)D(1));
        renderer_asl_word(&D(1),4); SET_W(D(2),0); flags_logic_w(0); break;
    case INDEXED_POSE_VALUE: SET_W(D(3),value); flags_logic_w(value); break;
    case INDEXED_POSE_COMPARE: step_compare_word(D(2),D(1)); break;
    case INDEXED_POSE_ADVANCE: step_add_word(&D(2),0x10); break;
    case INDEXED_POSE_TERMINATOR: step_add_word(&D(3),1); break;
    case INDEXED_GATE_ADDRESS: A(0)=address; break;
    }
}
static const IndexedCommandHooks hooks={mode_changed,outputs,NULL};
const IndexedCommandHooks *glue_indexed_command_hooks(void) { return &hooks; }
uint32_t glue_execute_indexed_command(const CommandRequest *request) {
    return execute_indexed_command(request,(int16_t)D(4),&hooks);
}
