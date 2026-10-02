/* C0EFD4/C0D730: source register and stack effects around readable owners. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_child_call.h"
#include "update_sequence.h"
#include "globals.h"

static const struct { uint32_t entry, return_pc; } children[] = {
    {0xc0f3c4,0xc0efe4}, {0xc0f5f8,0xc0efea}, {0xc11b44,0xc0eff0},
    {0xc12098,0xc0f008}, {0xc25b1e,0xc0f016}, {0xc1c63e,0xc0f01c},
    {0xc25b1c,0xc0f02a}, {0xc2d99c,0xc0f030}, {0xc1c54e,0xc0f036},
    {0xc254e8,0xc0f03c}, {0xc122a2,0xc0f042}, {0xc1c860,0xc0f048},
    {0xc2559a,0xc0f056}, {0xc25864,0xc0f05c}, {0xc0d730,0xc0f062},
    {0xc2aa9c,0xc0f082}, {0xc0daee,0xc0f090}, {0xc1cb14,0xc0f0ac},
    {0xc1cb26,0xc0f0ba}, {0xc279d0,0xc0f0c8}, {0xc1518c,0xc0f0de},
    {0xc265e8,0xc0f0e6}, {0xc1518c,0xc0f0f8}, {0xc1ccbc,0xc0f106},
    {0xc1ccbc,0xc0f116}, {0xc1518c,0xc0f124}, {0xc11bfc,0xc0f132},
    {0xc12950,0xc0f138}, {0xc30764,0xc0f182}, {0xc309b6,0xc0f188},
    {0xc31226,0xc0f18e}, {0xc332bc,0xc0f194}, {0xc3112a,0xc0f19a},
    {0xc30f78,0xc0f1a0}, {0xc31eb6,0xc0f1be}, {0xc31f4c,0xc0f1c4},
    {0xc32178,0xc0f1e2}, {0xc3201a,0xc0f20c}, {0xc3212a,0xc0f22a},
    {0xc30918,0xc0f248}, {0xc3003a,0xc0f256}, {0xc328a8,0xc0f25c},
    {0xc30a00,0xc0f262}, {0xc321d2,0xc0f268}, {0xc32260,0xc0f26e},
    {0xc31acc,0xc0f274}, {0xc30d34,0xc0f27a}, {0xc31a64,0xc0f280},
    {0xc322ee,0xc0f286}, {0xc30b5c,0xc0f28c}, {0xc31f4c,0xc0f2be},
    {0xc3201a,0xc0f2c4}, {0xc31eb6,0xc0f2ca}, {0xc322ee,0xc0f2dc},
    {0xc12242,0xc0f2ea}, {0xc31f4a,0xc0f2f0}, {0xc2b564,0xc0f2f6},
    {0xc25312,0xc0f2fc}, {0xc2548a,0xc0f30e}, {0xc53f9c,0xc0f322},
    {0xc2f582,0xc0f336}, {0xc082b8,0xc0f356}, {0xc12950,0xc0f376},
    {0xc53f9c,0xc0f37e}, {0xc2b3c2,0xc0f386}, {0xc2f49c,0xc0f3a4},
    {0xc31b76,0xc0f3b2}, {0xc32cee,0xc0f3c0},
    {0xc2fd8c,0xc0d742}, {0xc0da38,0xc0d748}
};
_Static_assert(sizeof children/sizeof children[0]==UPDATE_SEQUENCE_CHILD_COUNT,
               "Every source update call site must retain its own boundary");
typedef struct { int frame_exited; } UpdateCPU;
static UpdateSequenceResult consume(void *context,enum UpdateSequenceChild child) {
    UpdateCPU *cpu=context;
    UpdateSequenceResult result={0,0};
    if(child==UPDATE_BUFFERS || child==UPDATE_DISPLAY_END) {
        result.value=(uint32_t)glue_complete_child_or_frame_exit(
            children[child].entry,children[child].return_pc,
            rd_u32(A(6)+4),A(6)+8,&cpu->frame_exited);
        result.owner_finished=cpu->frame_exited;
    } else result.value=(uint32_t)glue_complete_child(children[child].entry,children[child].return_pc);
    return result;
}
static void outputs(void *context,const UpdateSequenceEvent *e) {
    (void)context;
    switch(e->phase) {
    case UPDATE_SEQUENCE_BEGIN:
        m68ki_push_32(A(6)); A(6)=A(7); A(7)-=2;
        wr_u16(A(6)-2,(uint16_t)e->value); flags_logic_w(e->value); break;
    case UPDATE_SEQUENCE_MARKER: flags_logic_w(e->value); break;
    case UPDATE_SEQUENCE_BYTE_TEST: flags_logic_b(e->value); break;
    case UPDATE_SEQUENCE_BYTE_LOAD: SET_B(D(0),e->value); flags_logic_b(D(0)); break;
    case UPDATE_SEQUENCE_LONG_COMPARE: step_compare_long(e->limit,e->value); break;
    case UPDATE_SEQUENCE_DECISION: flags_logic_l(e->value); break;
    case UPDATE_SEQUENCE_RECORD:
        SET_W(D(0),e->value); D(1)=9;
        D(0)=(uint32_t)(int32_t)(int16_t)D(0); step_asl_long(&D(0),D(1));
        A(0)=D(0)+CONTROL_RECORDS; SET_B(D(0),e->limit); step_compare_byte(0x30,D(0)); break;
    case UPDATE_SEQUENCE_TICK_COMPARE: case UPDATE_SEQUENCE_TICK_SUBTRACT:
        SET_W(D(0),e->value&e->mask); flags_logic_w(D(0));
        if(e->phase==UPDATE_SEQUENCE_TICK_SUBTRACT) step_subtract_word(&D(0),e->limit);
        else step_compare_word(e->limit,D(0)); break;
    case UPDATE_SEQUENCE_ACTIVITY_DECREMENT:
        step_subtract_byte(&D(0),1); flags_logic_b(D(0)); break;
    case UPDATE_SEQUENCE_CONTEXT_LATCH: flags_logic_b(1); break;
    case UPDATE_SEQUENCE_CONTEXT_MODE:
        SET_B(D(0),e->value); step_subtract_byte(&D(0),2); break;
    case UPDATE_SEQUENCE_ZERO_ARGUMENT:
        A(7)-=4; cache_step_write_memory(A(7),0,4,1); flags_logic_l(0); break;
    case UPDATE_SEQUENCE_DROP_ARGUMENT: A(7)+=4; break;
    case UPDATE_SEQUENCE_INCREMENT:
        SET_W(D(0),e->value); step_add_word(&D(0),1); flags_logic_w(D(0)); break;
    case UPDATE_SEQUENCE_END: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    case UPDATE_SEQUENCE_DISPLAY_FLAGS:
        SET_W(D(0),e->value&0x2000u); flags_logic_w(D(0)); break;
    }
}
int glue_C0EFD4(void) {
    UpdateCPU cpu={0}; UpdateSequenceHooks hooks={consume,outputs,&cpu};
    run_game_update_sequence(&hooks); return cpu.frame_exited?FA18_RET:glue_return();
}
int glue_C0D730(void) {
    UpdateCPU cpu={0}; UpdateSequenceHooks hooks={consume,outputs,&cpu};
    submit_update_display_buffers(&hooks); return cpu.frame_exited?FA18_RET:glue_return();
}
