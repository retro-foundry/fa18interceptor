/* C0F5F8-C0F810: offset accounting, phase selection and one stage tick. */
#include "post_input_tick.h"
#include "fault.h"
#include "globals.h"
#include <stdlib.h>

#define TICK_PRIMARY_OFFSET    0xc45914u
#define TICK_SECONDARY_OFFSET  0xc45904u
#define TICK_TERTIARY_OFFSET   0xc45908u
#define TICK_FIXED_OFFSET      0xc45910u
#define TICK_EXTRA_OFFSET      0xc4590cu
#define TICK_OFFSET_TARGET     0xc1ab74u
#define TICK_CLOCK             0xc457c1u

static void observe(const PostInputTickHooks *hooks, enum PostInputTickPhase phase,
                    uint32_t offset, uint32_t term, gaddr routine, uint16_t countdown) {
    PostInputTickEvent event={phase,offset,term,routine,countdown};
    if(hooks->observe) hooks->observe(hooks->context,&event);
}
static void account_offset(const PostInputTickHooks *hooks) {
    uint32_t offset=rd_u32(READOUT_SAMPLE)-rd_u32(TICK_PRIMARY_OFFSET)
                    -rd_u32(TICK_TERTIARY_OFFSET), term;
    observe(hooks,POST_TICK_OFFSET_BEGIN,offset,0,0,0);
    if(rd_u32(TICK_SECONDARY_OFFSET)) {
        term=rd_u32(READOUT_SAMPLE)-rd_u32(TICK_SECONDARY_OFFSET);
        offset-=term;
        observe(hooks,POST_TICK_OFFSET_SECONDARY,offset,term,0,0);
    }
    offset-=rd_u32(TICK_FIXED_OFFSET);
    observe(hooks,POST_TICK_OFFSET_FIXED,offset,0,0,0);
    if(rd_u32(TICK_EXTRA_OFFSET)) {
        term=rd_u32(READOUT_SAMPLE)-rd_u32(TICK_EXTRA_OFFSET);
        offset-=term;
        observe(hooks,POST_TICK_OFFSET_EXTRA,offset,term,0,0);
    }
    /* The stored long is tested as signed; all preceding arithmetic wraps.
     * Source C0F69A uses hexadecimal $4650 (18,000), not decimal 4,650. */
    if((int32_t)offset<0 || (int32_t)offset>=0x4650) {
        wr_u16(ERROR_CODE,0x3f);
        fault_hook();
    } else {
        gaddr target=rd_u32(TICK_OFFSET_TARGET);
        observe(hooks,POST_TICK_OFFSET_ACCEPTED,offset,0,0,0);
        wr_u32(target+8,rd_u32(target+8)+offset);
        wr_u8(MODE_TABLE_CHANGED,1);
    }
    wr_u32(TICK_PRIMARY_OFFSET,0);
}
static void select_phase(const PostInputTickHooks *hooks) {
    uint8_t phase=rd_u8(SEQUENCE_PHASE);
    if(rd_s8(PLAYER_PHASE)>=0) {
        if(phase==0xff) {
            observe(hooks,POST_TICK_PHASE_RESET,0,0,0,0);
            wr_u8(SEQUENCE_PHASE,0); wr_u8(SEQUENCE_FLAG,0);
            wr_u32(STAGE_CALLBACK,ROUTINE_END_SEQUENCE);
        } else if(phase==1) {
            wr_u8(SEQUENCE_PHASE,0);
            if(rd_u8(CONTEXT_REQUEST)) wr_u8(SEQUENCE_FLAG,1);
            wr_u8(POST_INPUT_EVENT,1); wr_u16(POST_INPUT_COUNTDOWN,3);
            observe(hooks,POST_TICK_PHASE_RESET,0,0,0,0);
            wr_u8(POST_INPUT_AUX,0); wr_u8(VIEWPORT_TARGET,0);
            wr_u32(STAGE_CALLBACK,ROUTINE_AWAIT_VIEWPORT);
        } else if(phase==2) {
            observe(hooks,POST_TICK_PHASE_RESET,0,0,0,0);
            wr_u8(SEQUENCE_FLAG,0);
            if(rd_s8(SEQUENCE_STEP)<0) {
                wr_u8(PLAYER_PHASE,0xf0); wr_u8(SEQUENCE_PHASE,0);
                observe(hooks,POST_TICK_PHASE_START,0,0,0,0);
                wr_u8(CONTEXT_GATE,1); wr_u8(POST_INPUT_EVENT,1);
                wr_u16(POST_INPUT_COUNTDOWN,6); wr_u32(STAGE_CALLBACK,0xc1104cu);
            }
        }
    } else if(phase==3) {
        /* MOVEQ zero occurs even when the signed step blocks this route. */
        observe(hooks,POST_TICK_PHASE_RESET,0,0,0,0);
        wr_u8(SEQUENCE_FLAG,0);
        if(rd_s8(SEQUENCE_STEP)<0) {
            wr_u8(SEQUENCE_PHASE,0);
            wr_u16(POST_INPUT_COUNTDOWN,rd_u16(PHASE_WORD));
            wr_u32(STAGE_CALLBACK,0xc11078u);
        }
    }
}
void run_post_input_tick(const PostInputTickHooks *hooks) {
    uint16_t countdown;
    gaddr routine;
    if(!hooks || !hooks->consume) abort();
    if(rd_s8(ATTEMPTS_LEFT)>=0 && rd_s8(MODE_SELECT)>0) {
        if(rd_u8(SEQUENCE_PHASE) && !rd_u8(RECORDER_ON)
            && rd_s8(SEQUENCE_STEP)<0 && rd_u32(TICK_PRIMARY_OFFSET))
            account_offset(hooks);
        select_phase(hooks);
    }
    if(rd_s8(SEQUENCE_STEP)>=0)
        wr_u8(SEQUENCE_STEP,(uint8_t)(rd_u8(SEQUENCE_STEP)-1u));
    wr_u8(TICK_CLOCK,(uint8_t)(rd_u8(TICK_CLOCK)+1u));
    countdown=rd_u16(POST_INPUT_COUNTDOWN);
    wr_u16(POST_INPUT_COUNTDOWN,(uint16_t)(countdown-1u));
    routine=rd_u32(STAGE_CALLBACK);
    observe(hooks,POST_TICK_DISPATCH,0,0,routine,countdown);
    hooks->consume(hooks->context,routine);
    /* The child owns any changes it made; only this final byte is cleared. */
    wr_u8(KEY_TAKEN,0);
}
