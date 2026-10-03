#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_record_control_actions.h"
#include "record_control_actions.h"
static void full(unsigned reg,uint32_t v) { D(reg)=v; flags_logic_l(v); }
static void partial(unsigned reg,uint16_t v) { SET_W(D(reg),v); flags_logic_w(v); }
static void extended(unsigned reg) { full(reg,(uint32_t)(int32_t)(int16_t)D(reg)); }
static void push(uint32_t v) { m68ki_push_32(v); flags_logic_l(v); }
static uint32_t consume(void *context,enum RecordActionChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc266ae,0xc15450},{0xc26c72,0xc154c0},{0xc2cd94,0xc15600},{0xc2cd28,0xc1561e},{0xc2cca0,0xc15630},
        {0xc159ae,0xc158ce},{0xc15ad4,0xc15ac8},{0xc257ec,0xc15b02},{0xc17b08,0xc181b4},{0xc17b2c,0xc181f2}
    };
    unsigned arguments; (void)context;
    if(child<=RA_INITIALISE_DIRECTION) { push(D(0)); arguments=4; }
    else if(child==RA_TRANSFORM_DIRECTION) { push(D(3)); push(D(0)); push(D(1)); push(D(2)); arguments=16; }
    else if(child==RA_PROJECT_DIRECTION) { push(D(3)); push(D(2)); push(D(1)); push(D(0)); arguments=16; }
    else if(child==RA_RESERVE_ALERT) { full(0,2); push(D(0)); arguments=4; }
    else { m68ki_push_32(0); flags_logic_l(0); full(0,2); push(D(0)); full(0,10); push(D(0)); arguments=12; }
    glue_complete_child(sites[child].entry,sites[child].ret); A(7)+=arguments; return D(0);
}
/* Preserve the original matrix evaluation order and partial register halves.
 * Domain code owns the resulting vector stores. No child occurs within these
 * arithmetic phases, and source constants and coefficients remain original. */
static void launch_position(uint32_t frame) {
    partial(0,rd_u16(0xc46216)); partial(1,rd_u16(frame-2)); renderer_multiply(&D(0),D(1));
    partial(2,rd_u16(0xc46218)); partial(3,rd_u16(frame-4)); renderer_multiply(&D(2),D(3)); step_add_long(&D(0),D(2));
    partial(2,rd_u16(0xc4621a)); partial(4,rd_u16(frame-6)); renderer_multiply(&D(2),D(4)); step_add_long(&D(0),D(2)); step_asr_long(&D(0),6); flags_logic_l(D(0));
    partial(2,rd_u16(0xc4621c)); renderer_multiply(&D(2),D(1)); partial(5,rd_u16(0xc4621e)); renderer_multiply(&D(5),D(3)); step_add_long(&D(2),D(5));
    partial(5,rd_u16(0xc46220)); renderer_multiply(&D(5),D(4)); step_add_long(&D(2),D(5)); step_asr_long(&D(2),6); flags_logic_l(D(2));
    partial(0,rd_u16(0xc46222)); renderer_multiply(&D(1),D(0)); partial(0,rd_u16(0xc46224)); renderer_multiply(&D(3),D(0)); step_add_long(&D(1),D(3));
    partial(0,rd_u16(0xc46226)); renderer_multiply(&D(4),D(0)); step_add_long(&D(1),D(4)); step_asr_long(&D(1),6); flags_logic_l(D(1));
}
static void launch_velocity(uint32_t frame) {
    partial(0,rd_u16(0xc46218)); partial(1,rd_u16(frame-4)); renderer_multiply(&D(0),D(1));
    partial(2,rd_u16(0xc4621a)); partial(3,rd_u16(frame-6)); renderer_multiply(&D(2),D(3)); step_add_long(&D(0),D(2)); step_asr_long(&D(0),4); flags_logic_l(D(0));
    partial(0,rd_u16(0xc4621e)); renderer_multiply(&D(0),D(1)); partial(2,rd_u16(0xc46220)); renderer_multiply(&D(2),D(3)); step_add_long(&D(0),D(2)); step_asr_long(&D(0),4); flags_logic_l(D(0));
    partial(0,rd_u16(0xc46224)); renderer_multiply(&D(1),D(0)); partial(0,rd_u16(0xc46226)); renderer_multiply(&D(3),D(0)); step_add_long(&D(1),D(3)); step_asr_long(&D(1),4); flags_logic_l(D(1));
}
static void target_relative(uint32_t frame) {
    full(0,rd_u32(0xc46198)); D(0)&=0x3fffff; flags_logic_l(D(0)); full(1,rd_u32(0xc45a52)); step_add_long(&D(1),D(0)); A(0)=rd_u32(0xc18214); step_subtract_long(&D(1),rd_u32(A(0)));
    full(0,rd_u32(0xc45a56)); step_add_long(&D(0),rd_u32(0xc4619c)); step_subtract_long(&D(0),rd_u32(A(0)+4));
    full(2,rd_u32(0xc461a0)); D(2)&=0x3fffff; flags_logic_l(D(2)); full(3,rd_u32(0xc45a5a)); step_add_long(&D(3),D(2)); step_subtract_long(&D(3),rd_u32(A(0)+8));
    flags_logic_l(D(1)); step_asr_long(&D(1),8); flags_logic_l(D(0)); step_asr_long(&D(0),8); flags_logic_l(D(3)); step_asr_long(&D(3),8);
    partial(2,rd_u16(frame-2)); extended(2); flags_logic_w(D(1)); extended(1); flags_logic_w(D(0)); extended(0); flags_logic_w(D(3)); extended(3);
}
static void outputs(void *context,enum RecordActionPhase p,uint32_t v,uint32_t other) {
    uint32_t temporary; (void)context;
    switch(p) {
    case RA_PRIMARY_BYTE: SET_B(D(0),v); flags_logic_b(v); break;
    case RA_PRIMARY_WORD: partial(0,v); break;
    case RA_PRIMARY_LONG: full(0,v); break;
    case RA_SECONDARY_BYTE: SET_B(D(1),v); flags_logic_b(v); break;
    case RA_SECONDARY_WORD: partial(1,v); break;
    case RA_SECONDARY_LONG: full(1,v); break;
    case RA_LOOKUP: A(0)=v; break;
    case RA_OTHER_RECORD: case RA_NEXT_OTHER_RECORD: A(1)=v; break;
    case RA_HEIGHT_CURSOR: A(2)=v; break;
    case RA_STORE_BYTE: flags_logic_b(v); break;
    case RA_STORE_WORD: case RA_TEST_WORD: flags_logic_w(v); break;
    case RA_STORE_LONG: case RA_TEST_LONG: flags_logic_l(v); break;
    case RA_COMPARE_BYTE: step_compare_byte(other,v); break;
    case RA_COMPARE_WORD: step_compare_word(other,v); break;
    case RA_COMPARE_LONG: step_compare_long(other,v); break;
    case RA_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case RA_PRIMARY_EXT_LONG: full(0,v); break;
    case RA_PRIMARY_AND_WORD: partial(0,D(0)&v); break;
    case RA_PRIMARY_AND_LONG: full(0,D(0)&v); break;
    case RA_PRIMARY_OR_WORD: partial(0,D(0)|v); break;
    case RA_PRIMARY_ADD_WORD: step_add_word(&D(0),v); break;
    case RA_PRIMARY_SUB_WORD: step_subtract_word(&D(0),v); break;
    case RA_PRIMARY_ADD_LONG: step_add_long(&D(0),v); break;
    case RA_PRIMARY_ASR_WORD: renderer_asr_word(&D(0),v); break;
    case RA_PRIMARY_ASR_LONG: step_asr_long(&D(0),v); break;
    case RA_PRIMARY_ASL_LONG: step_asl_long(&D(0),v); break;
    case RA_SECONDARY_EXT_WORD: partial(1,v); break;
    case RA_SECONDARY_EXT_LONG: full(1,v); break;
    case RA_SECONDARY_ADD_LONG: step_add_long(&D(1),v); break;
    case RA_SECONDARY_ASL_LONG: step_asl_long(&D(1),v); break;
    case RA_MEMORY_ADD_WORD: temporary=v; step_add_word(&temporary,other); break;
    case RA_MEMORY_SUB_WORD: temporary=v; step_subtract_word(&temporary,other); break;
    case RA_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break;
    case RA_MEMORY_SUB_LONG: temporary=v; step_subtract_long(&temporary,other); break;
    case RA_MEMORY_NEG_WORD: temporary=v; renderer_negate(&temporary,2); break;
    case RA_ORIENTATION_BASE: full(1,9); extended(0); step_asl_long(&D(0),D(1)); A(0)=D(0)+0xc46184; break;
    case RA_LAUNCH_POSITION: launch_position(v); break;
    case RA_LAUNCH_VELOCITY: launch_velocity(v); break;
    case RA_TARGET_RELATIVE: target_relative(v); break;
    case RA_DIRECTION_ARGUMENTS:
        partial(0,rd_u16(v+10)); extended(0); partial(1,rd_u16(v+14)); extended(1);
        partial(2,rd_u16(v+18)); extended(2); partial(3,rd_u16(v+22)); extended(3); break;
    }
}
static const RecordActionHooks hooks={consume,outputs,NULL};
static void frame_begin(unsigned bytes,uint16_t saved) {
    m68ki_push_32(A(6)); A(6)=A(7); A(7)-=bytes;
    if(saved) renderer_store(A(7),saved,4,7);
}
static int frame_end(uint16_t saved) {
    if(saved) renderer_load(A(7),saved,4,7);
    A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return();
}
int glue_C153FC(void) { frame_begin(18,0x20); advance_control_record_action(A(6),&hooks); return frame_end(0x400); }
int glue_C15688(void) { frame_begin(10,0x3c00); initialise_control_record_action(A(6),&hooks); return frame_end(0x3c); }
int glue_C159AE(void) { frame_begin(24,0x3000); aim_control_record_action(A(6),&hooks); return frame_end(0x0c); }
int glue_C15AD4(void) { frame_begin(12,0x3000); publish_control_record_direction(A(6),&hooks); return frame_end(0x0c); }
int glue_C181A0(void) { frame_begin(0,0); start_control_record_alert(A(6),&hooks); return frame_end(0); }
int glue_C15138(void) { frame_begin(4,0); attenuate_control_record_offset(A(6),&hooks); return frame_end(0); }
