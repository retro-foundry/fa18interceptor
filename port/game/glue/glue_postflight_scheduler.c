#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_postflight_scheduler.h"
#include "postflight_scheduler.h"

static PostflightScheduleResult consume(void *context,enum PostflightScheduleChild child,gaddr record) {
    static const struct {uint32_t entry,ret;} calls[]={
        {0xc230b0,0xc09e1e},{0xc09e98,0xc09e46},{0xc09ec4,0xc09e52},
        {0xc0a002,0xc09e5e},{0xc0a15c,0xc09e6a},{0xc0a1e0,0xc09e76},
        {0xc0a2f0,0xc09e82},{0xc0a334,0xc09e8e},{0xc0a364,0xc09e94},
        {0xc0a3ea,0xc0a3da},{0xc1bee8,0xc09fbc},{0xc1bee8,0xc0a2bc},
        {0xc0a12e,0xc0a0b8},{0xc0a12e,0xc0a0be}
    };
    PostflightScheduleResult result;
    (void)context; (void)record;
    glue_complete_child(calls[child].entry,calls[child].ret);
    result.event=D(0); result.zero=COND_EQ(); return result;
}
static void outputs(void *context,enum PostflightSchedulePhase phase,uint32_t value,
                    uint32_t limit,gaddr address) {
    uint32_t temporary;
    (void)context;
    switch(phase) {
    case SCHEDULE_BYTE_TEST: case SCHEDULE_BYTE_STORE: flags_logic_b(value); break;
    case SCHEDULE_WORD_TEST: case SCHEDULE_WORD_STORE: flags_logic_w(value); break;
    case SCHEDULE_LONG_STORE: flags_logic_l(value); break;
    case SCHEDULE_BIT_TEST: case SCHEDULE_BIT_SET: FLAG_Z=value&(1u<<limit); break;
    case SCHEDULE_BYTE_COMPARE: step_compare_byte(limit,value); break;
    case SCHEDULE_WORD_COMPARE: step_compare_word(limit,value); break;
    case SCHEDULE_LONG_COMPARE: step_compare_long(limit,value); break;
    case SCHEDULE_BYTE_DECREMENT: temporary=value; step_subtract_byte(&temporary,1); break;
    case SCHEDULE_WORD_DECREMENT: temporary=value; step_subtract_word(&temporary,1); break;
    case SCHEDULE_WORD_OR:
        if(address) flags_logic_w(value|limit);
        else { SET_W(D(0),value); flags_logic_w(value); }
        break;
    case SCHEDULE_EVENT_BYTE: SET_B(D(0),value); flags_logic_b(value); break;
    case SCHEDULE_EVENT_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case SCHEDULE_EVENT_MASK_BYTE: SET_B(D(0),value); flags_logic_b(value); break;
    case SCHEDULE_EVENT_MASK_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case SCHEDULE_EVENT_ZERO: case SCHEDULE_EVENT_ONE: case SCHEDULE_EVENT_FOUR: case SCHEDULE_EVENT_SEVEN:
        D(0)=value; flags_logic_l(value); break;
    case SCHEDULE_EVENT_DECREMENT: step_subtract_byte(&D(0),1); break;
    case SCHEDULE_RECORD: A(1)=address; break;
    case SCHEDULE_PAIR: A(2)=address; A(3)=address+0x400; break;
    case SCHEDULE_PAIR_SWAP: temporary=A(2); A(2)=A(3); A(3)=temporary; break;
    case SCHEDULE_RESTORE_RECORD_SELECT: A(1)=address; break;
    case SCHEDULE_TARGET_RECORD: A(1)=address; break;
    case SCHEDULE_PLAYER_RECORD: A(0)=address; break;
    case SCHEDULE_POSITION_LOAD: renderer_load(address,7,4,-1); break;
    case SCHEDULE_REFERENCE_LOAD: renderer_load(address,0x38,4,-1); break;
    case SCHEDULE_DISTANCE_LIMIT: D(6)=value; flags_logic_l(value); break;
    case SCHEDULE_DISTANCE_SUBTRACT: step_subtract_long(&D(3+limit),value); break;
    case SCHEDULE_DISTANCE_NEGATE: renderer_negate(&D(3+limit),4); break;
    case SCHEDULE_DISTANCE_COMPARE: step_compare_long(limit,value); break;
    case SCHEDULE_TARGET_POSITION: renderer_load(address,7,4,-1); break;
    case SCHEDULE_TARGET_SUBTRACT_X: step_subtract_long(&D(0),value); break;
    case SCHEDULE_TARGET_SUBTRACT_Z: step_subtract_long(&D(2),value); break;
    case SCHEDULE_TARGET_NEGATE_X: renderer_negate(&D(0),4); break;
    case SCHEDULE_TARGET_NEGATE_Z: renderer_negate(&D(2),4); break;
    case SCHEDULE_TARGET_COMPARE_X: case SCHEDULE_TARGET_COMPARE_Z: step_compare_long(limit,value); break;
    case SCHEDULE_RESTORE_KIND: SET_B(D(0),value); flags_logic_b(value); step_add_word(&D(0),D(0)); break;
    case SCHEDULE_RESTORE_TABLE:
        A(4)=address; A(4)+=(uint32_t)(int32_t)rd_s16(address+(gaddr)(int32_t)(int16_t)D(0)); break;
    case SCHEDULE_RESTORE_VALUES: renderer_load(address,0x7c,2,4); break;
    case SCHEDULE_PREPARE_INDEX: SET_W(D(1),value); flags_logic_w(value); break;
    }
}
static const PostflightScheduleHooks hooks={consume,outputs,NULL};
static int run(enum PostflightSchedule mode) {
    schedule_postflight(mode,(uint16_t)D(0),A(1),&hooks); return glue_return();
}
int glue_C09E06(void) { return run(POSTFLIGHT_DISPATCH); }
int glue_C09E98(void) { return run(POSTFLIGHT_MODE_THREE); }
int glue_C09EC4(void) { return run(POSTFLIGHT_MODE_FOUR); }
int glue_C0A002(void) { return run(POSTFLIGHT_MODE_FIVE); }
int glue_C0A12E(void) { return run(POSTFLIGHT_RESTORE_RECORD); }
int glue_C0A15C(void) { return run(POSTFLIGHT_MODE_SIX); }
int glue_C0A1E0(void) { return run(POSTFLIGHT_MODE_SEVEN); }
int glue_C0A2F0(void) { return run(POSTFLIGHT_MODE_NINE); }
int glue_C0A334(void) { return run(POSTFLIGHT_MODE_125); }
int glue_C0A364(void) { return run(POSTFLIGHT_MODE_OTHER); }
int glue_C0A3EA(void) { return run(POSTFLIGHT_PLAYER_READY); }
