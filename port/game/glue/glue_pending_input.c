/* C0F3C4: exact frame locals, high words and original child boundaries. */
#include "glue_step.h"
#include "glue_child_call.h"
#include "pending_input.h"
#include <stdlib.h>
static const struct { uint32_t entry,return_pc; } children[] = {
    {0xc16eae,0xc0f3ce}, {0xc1715c,0xc0f3d4}, {0xc13d34,0xc0f3f4},
    {0xc1ac28,0xc0f416}, {0xc1ac28,0xc0f438}, {0xc16c56,0xc0f440},
    {0xc1ad74,0xc0f45c}
};
static uint32_t consume(void *context,enum PendingInputChild child,uint8_t key) {
    (void)context;
    if(child==PENDING_DISPATCH_KEY && rd_u32(A(7))!=key) abort();
    return (uint32_t)glue_complete_child(children[child].entry,children[child].return_pc);
}
static void outputs(void *context,const PendingInputEvent *e) {
    (void)context;
    switch(e->phase) {
    case PENDING_BEGIN: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=6; break;
    case PENDING_INPUT_WORD:
        SET_W(D(1),e->previous); wr_u16(A(6)-6,(uint16_t)e->value);
        step_compare_word(D(0),D(1)); break;
    case PENDING_CHANGED_WORD: flags_logic_w(e->value); break;
    case PENDING_MODE:
        SET_B(D(0),e->value); wr_u8(A(6)-2,(uint8_t)D(0)); step_compare_byte(1,D(0));
        if(e->value!=1) step_subtract_byte(&D(0),2); break;
    case PENDING_MODE_THREE: step_compare_byte(3,e->value); break;
    case PENDING_WORD_TEST: flags_logic_w(e->value); break;
    case PENDING_WORD_PAIR:
        SET_W(D(0),e->value|e->previous); SET_W(D(1),e->previous); flags_logic_w(D(0)); break;
    case PENDING_KEY:
        wr_u8(A(6)-1,(uint8_t)e->value); step_compare_byte(0xff,D(0)); break;
    case PENDING_ARGUMENT:
        wr_u8(A(6)-3,(uint8_t)e->value); D(0)&=0xffu;
        m68ki_push_32(D(0)); flags_logic_l(D(0)); break;
    case PENDING_DROP_ARGUMENT: A(7)+=4; break;
    case PENDING_MODE_END: flags_logic_b(e->value); break;
    case PENDING_MODE_ONE: step_compare_byte(1,e->value); break;
    case PENDING_COPY_WORD: flags_logic_w(e->value); break;
    case PENDING_CLEAR_WORDS: D(0)=0; flags_logic_w(0); break;
    case PENDING_CLEAR_SECOND: flags_logic_w(0); break;
    case PENDING_END: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    }
}
int glue_C0F3C4(void) {
    PendingInputHooks hooks={consume,outputs,NULL};
    process_pending_key_events(&hooks); return glue_return();
}
