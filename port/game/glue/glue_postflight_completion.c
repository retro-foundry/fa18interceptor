#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_postflight_completion.h"
#include "postflight_completion.h"
#include "globals.h"
#include "glue_text.h"
static void consume(void *context,enum PostflightCompletionChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc092a0,0xc117d2},{0xc2fd22,0xc1180a},{0xc1b906,0xc11858},
        {0xc092a0,0xc1185e},{0xc11acc,0xc118b4}
    };
    (void)context;
    if(child==PFC_LOAD_TABLE) m68ki_push_32(0xc08490);
    glue_complete_child(sites[child].entry,sites[child].ret);
    if(child==PFC_LOAD_TABLE) A(7)+=4;
}
static void outputs(void *context,enum PostflightCompletionPhase p,uint32_t value,uint32_t other) {
    (void)context;
    switch(p) {
    case PFC_D0_BYTE: SET_B(D(0),value); flags_logic_b(value); break;
    case PFC_D0_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case PFC_D0_ZERO: D(0)=0; flags_logic_l(0); break;
    case PFC_D1_BYTE: SET_B(D(1),value); flags_logic_b(value); break;
    case PFC_TEST_BYTE: case PFC_BYTE_STORE: flags_logic_b(value); break;
    case PFC_TEST_WORD: case PFC_WORD_STORE: flags_logic_w(value); break;
    case PFC_CALLBACK_LEA: A(0)=value; flags_logic_l(value); break;
    case PFC_CALLBACK_DIRECT: flags_logic_l(value); break;
    case PFC_AND_WORD: SET_W(D(0),D(0)&value); flags_logic_w(D(0)); break;
    case PFC_OR_WORD: SET_W(D(0),D(0)|value); flags_logic_w(D(0)); break;
    case PFC_COMPARE_BYTE: step_compare_byte((uint8_t)other,(uint8_t)value); break;
    case PFC_VIEWPORT_COMPARE: step_compare_byte((uint8_t)other,(uint8_t)value); break;
    case PFC_SUB_BYTE: step_subtract_byte(&D(0),(uint8_t)value); break;
    case PFC_BIT_WORD: FLAG_Z=value&(1u<<other); break;
    }
}
static const PostflightCompletionHooks hooks={consume,outputs,NULL};
int glue_C11788(void) { advance_postflight_completion(&hooks); return glue_return(); }
int glue_C11830(void) { restart_postflight_completion(&hooks); return glue_return(); }
int glue_C11872(void) { expire_postflight_completion(&hooks); return glue_return(); }
int glue_C118A0(void) { queue_postflight_failure(&hooks); return glue_return(); }
int glue_C118E6(void) { end_postflight_message(&hooks); return glue_return(); }
int glue_C118FC(void) { follow_postflight_message(&hooks); return glue_return(); }
int glue_C11934(void) { clear_postflight_phase(&hooks); return glue_return(); }
int glue_C11958(void) { follow_postflight_message_or_phase(&hooks); return glue_return(); }
int glue_C119D4(void) { restart_postflight_after_countdown(&hooks); return glue_return(); }
int glue_C1104C(void) { queue_postflight_end(&hooks); return glue_return(); }
int glue_C0F946(void) { await_postflight_viewport(&hooks); return glue_return(); }
int glue_C0F974(void) { mark_postflight_viewport_ready(&hooks); return glue_return(); }
/* C09192 calls the independently callable root-record entry C091E6.
 * The domain and product outputs reuse the existing local-to-world body. */
int glue_C091E6(void) {
    uint32_t position,temporary;
    m68ki_push_32(A(2)); m68ki_push_32(A(1));
    A(1)=CONTROL_RECORDS; A(2)=CONTROL_RECORDS+RECORD_INVERSE;
    world_registers(A(1),A(2));
    position=rd_u32(CONTROL_RECORDS+0x1c);
    temporary=D(2)-position; step_add_long(&temporary,position);
    A(1)=m68ki_pull_32(); A(2)=m68ki_pull_32(); return glue_return();
}
