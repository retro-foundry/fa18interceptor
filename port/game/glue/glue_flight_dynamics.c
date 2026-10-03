#include "glue_flight_dynamics_math.h"
#include "glue_flight_dynamics.h"
#include "glue_child_call.h"
#include "flight_dynamics.h"
static DynamicsState working(void) {
    DynamicsState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),COND_EQ()}; return w;
}
static DynamicsState consume(void *context,enum DynamicsChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2d970,0xc25b3a},{0xc28e28,0xc25bac},{0xc2c392,0xc25c70},{0xc1b27e,0xc25c76},
        {0xc25704,0xc25d20},{0xc25704,0xc25d54},{0xc13d84,0xc25d84},{0xc2d408,0xc25da4},
        {0xc149be,0xc25e2c},{0xc26ebe,0xc26014},{0xc17f8c,0xc260fe},{0xc25704,0xc2616a},
        {0xc06c02,0xc261d6},{0xc26322,0xc26246},{0xc2b05a,0xc2624e},{0xc26352,0xc2625e},{0xc2651e,0xc26282},
        {0xc26cc0,0xc26af2},{0xc26d8a,0xc26af8},{0xc28b16,0xc289da},{0xc28b16,0xc289f6},
        {0xc28f16,0xc28a6a},{0xc28f16,0xc28a98},{0xc28f16,0xc28d58},{0xc2d954,0xc28e02}
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret);
    if(child==DY_COLLISION_SOUND) A(7)+=8;
    return working();
}
static void outputs(void *context,enum DynamicsPhase phase,enum DynamicsValue field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case DY_BYTE: SET_B(D(r),v); flags_logic_b(v); break; case DY_WORD: SET_W(D(r),v); flags_logic_w(v); break;
    case DY_LONG: D(r)=v; flags_logic_l(v); break; case DY_POINTER: A(r-DY_ROOT)=v; break;
    case DY_ADD_BYTE: renderer_add_byte(&D(r),v); break; case DY_ADD_WORD: step_add_word(&D(r),v); break; case DY_ADD_LONG: step_add_long(&D(r),v); break;
    case DY_SUB_BYTE: step_subtract_byte(&D(r),v); break; case DY_SUB_WORD: step_subtract_word(&D(r),v); break; case DY_SUB_LONG: step_subtract_long(&D(r),v); break;
    case DY_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break; case DY_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break;
    case DY_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break; case DY_OR_BYTE: SET_B(D(r),D(r)|v); flags_logic_b(D(r)); break;
    case DY_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case DY_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case DY_SWAP: step_swap(&D(r)); break; case DY_ASR_WORD: renderer_asr_word(&D(r),v); break; case DY_ASR_LONG: step_asr_long(&D(r),v); break;
    case DY_ASL_WORD: renderer_asl_word(&D(r),v); break; case DY_ASL_LONG: step_asl_long(&D(r),v); break; case DY_LSR_WORD: flight_lsr_word(&D(r),v); break;
    case DY_ROL_LONG: dynamics_rol_long(&D(r),v); break; case DY_NEG_WORD: renderer_negate(&D(r),2); break; case DY_NEG_LONG: renderer_negate(&D(r),4); break;
    case DY_MULTIPLY: renderer_multiply(&D(r),(uint16_t)v); break;
    case DY_TEST_BYTE: case DY_STORE_BYTE: flags_logic_b(v); break; case DY_TEST_WORD: case DY_STORE_WORD: flags_logic_w(v); break;
    case DY_TEST_LONG: case DY_STORE_LONG: flags_logic_l(v); break;
    case DY_COMPARE_BYTE: step_compare_byte(other,v); break; case DY_COMPARE_WORD: step_compare_word(other,v); break; case DY_COMPARE_LONG: step_compare_long(other,v); break;
    case DY_BIT_TEST: case DY_BIT_SET: case DY_BIT_CLEAR: FLAG_Z=v&(1u<<other); break;
    case DY_REGISTER_BIT_SET: FLAG_Z=v&(1u<<other); D(r)|=1u<<other; break;
    case DY_REGISTER_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case DY_MEMORY_ADD_BYTE: temporary=v; renderer_add_byte(&temporary,other); break; case DY_MEMORY_ADD_WORD: temporary=v; step_add_word(&temporary,other); break;
    case DY_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break; case DY_MEMORY_SUB_WORD: temporary=v; step_subtract_word(&temporary,other); break;
    case DY_MEMORY_SUB_LONG: temporary=v; step_subtract_long(&temporary,other); break; case DY_DECREMENT: SET_W(D(r),v); break;
    case DY_LOAD_WORDS: renderer_load(v,(uint16_t)other,2,-1); break;
    case DY_LOAD_LONGS: renderer_load(v?v:A(7),(uint16_t)other,4,-1); break;
    case DY_STORE_WORDS: case DY_STORE_LONGS: break; /* Domain owns all RAM writes; MOVEM preserves flags. */
    case DY_SAVE_RECORD: m68ki_push_32(A(1)); flags_logic_l(A(1)); break; case DY_RESTORE_RECORD: A(1)=m68ki_pull_32(); break;
    case DY_SAVE_CELL: renderer_store(A(7),0xc0,4,7); break; case DY_RESTORE_CELL: renderer_load(A(7),0x300,4,7); break;
    case DY_SAVE_RATES: renderer_store(A(7),0x740,4,7); break; case DY_RESTORE_RATES: renderer_load(A(7),0x2e0,4,7); break;
    case DY_SAVE_ORIENTATION: renderer_store(A(7),0x80a0,4,7); break; case DY_RESTORE_ORIENTATION: renderer_load(A(7),0x501,4,7); break;
    case DY_SAVE_SCAN: m68ki_push_32(D(5)); flags_logic_l(D(5)); break; case DY_RESTORE_SCAN: D(5)=m68ki_pull_32(); flags_logic_l(D(5)); break;
    case DY_SOUND_ARGUMENTS: m68ki_push_32(0x30); flags_logic_l(0x30); m68ki_push_32(0x1c); flags_logic_l(0x1c); break;
    case DY_BEGIN_FRAME: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=v; break;
    case DY_END_FRAME: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    }
}
static gaddr frame(void *context) { (void)context; return A(6); }
static DynamicsState restored_state(void *context) { (void)context; return working(); }
static const DynamicsHooks hooks={consume,outputs,frame,restored_state,NULL};
int glue_C25B66(void) { advance_indexed_record_dynamics(working(),&hooks); return glue_return(); }
int glue_C266AE(void) { collide_scene_motion(working(),&hooks); return glue_return(); }
int glue_C28996(void) { update_scene_regions(working(),&hooks); return glue_return(); }
int glue_C28B16(void) { spawn_region_records(working(),&hooks); return glue_return(); }
int glue_complete_region_dispatch(void) { dispatch_region_records(working(),&hooks); return glue_return(); }
