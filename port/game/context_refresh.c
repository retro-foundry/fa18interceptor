/* C1C860-C1CA2C: request consumption and the ordered context-refresh stages. */
#include "context_refresh.h"
#include "control_records.h"
#include "fault.h"
#include "globals.h"
#include <stdlib.h>

#define REFRESH_PREPARED 0xc45859u
#define REFRESH_ALTERNATE 0xc45865u
#define REFRESH_ORIGIN_X 0xc45c3eu
#define REFRESH_ORIGIN_Z 0xc45c46u

static void observe(const ContextRefreshHooks *hooks,enum ContextRefreshPhase phase,
                    uint32_t value,uint32_t x,uint32_t z,gaddr record,unsigned bit,unsigned shift) {
    ContextRefreshEvent event={phase,value,x,z,record,bit,shift};
    if(hooks->observe) hooks->observe(hooks->context,&event);
}
static void refresh_templates(const ContextRefreshHooks *hooks,unsigned bit) {
    uint8_t requests=rd_u8(UPDATE_MASK);
    wr_u8(UPDATE_MASK,(uint8_t)(requests&~(1u<<bit)));
    if(bit==1) wr_u8(REFRESH_ALTERNATE,1);
    observe(hooks,CONTEXT_REFRESH_TEMPLATE_CALL,requests,0,0,0,bit,0);
    hooks->consume(hooks->context,CONTEXT_REFRESH_TEMPLATES);
}
void refresh_context_packet(const ContextRefreshHooks *hooks) {
    uint32_t guard=rd_u32(POSITION_BIAS);
    uint8_t requests;
    if(!hooks || !hooks->consume) abort();
    observe(hooks,CONTEXT_REFRESH_GUARD,guard,0,0,0,0,0);
    if((int32_t)guard<(int32_t)0xf8000000u) return;
    observe(hooks,CONTEXT_REFRESH_SAVE,0,0,0,0,0,0);
    observe(hooks,CONTEXT_REFRESH_FRAME_GATE,0,0,0,0,0,0);
    requests=rd_u8(UPDATE_MASK);
    observe(hooks,CONTEXT_REFRESH_REQUESTS,requests,0,0,0,0,0);
    if(requests) {
        uint32_t x,z;
        gaddr record=0;
        unsigned shift;
        uint8_t kind=requests&15u;
        wr_u16(CELL_TIMER,0x49);
        /* C1CA82 flags all records; it leaves the request byte in D0. */
        flag_all_records();
        observe(hooks,CONTEXT_REFRESH_FLAGGED,requests,0,0,0,0,0);
        if(kind!=0xc && kind!=0xb && kind!=0xf) {
            wr_u16(ERROR_CODE,0x27); fault_hook();
        }
        if(rd_u8(CONTEXT_SELECT)) {
            x=rd_u32(REFRESH_ORIGIN_X)&0x1fffffffu;
            z=rd_u32(REFRESH_ORIGIN_Z)&0x1fffffffu;
            x=(x<<16)|(x>>16); z=(z<<16)|(z>>16); shift=8;
        } else {
            record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(VIEW_RECORD);
            x=rd_u16(record+6); z=rd_u16(record+8); shift=2;
        }
        observe(hooks,CONTEXT_REFRESH_SELECTOR,requests,x,z,record,0,shift);
        wr_s16(CONDITION_KEY_A,(int16_t)((int16_t)x>>shift));
        wr_s16(CONDITION_KEY_B,(int16_t)((int16_t)z>>shift));
        wr_u8(REFRESH_PREPARED,1); wr_u8(CELL_CHECKS,0); wr_u8(REFRESH_ALTERNATE,0);
        if(requests&1u) refresh_templates(hooks,0);
        /* Each child owns changes to the remaining requests. Only bit zero
         * uses the originally loaded D0 byte; all later tests reread RAM. */
        if(rd_u8(UPDATE_MASK)&2u) refresh_templates(hooks,1);
        wr_u8(CELL_CHECKS,1);
        if(rd_u8(UPDATE_MASK)&4u) wr_u8(UPDATE_MASK,(uint8_t)(rd_u8(UPDATE_MASK)&~4u));
        if(rd_u8(UPDATE_MASK)&8u) {
            refresh_templates(hooks,3); wr_u16(CELL_TIMER,0x4a);
        }
        observe(hooks,CONTEXT_REFRESH_FRAME_GATE,1,0,0,0,0,0);
        wr_u8(UPDATE_MASK,0);
    }
    observe(hooks,CONTEXT_REFRESH_SORT_CALL,0,0,0,0,0,0);
    hooks->consume(hooks->context,CONTEXT_REFRESH_SORT);
    wr_u16(CELL_TIMER,0x4b);
    observe(hooks,CONTEXT_REFRESH_FRAME_GATE,0,0,0,0,0,0);
    observe(hooks,CONTEXT_REFRESH_CACHE_CALL,0,0,0,0,0,0);
    hooks->consume(hooks->context,CONTEXT_REFRESH_CACHE);
    wr_u16(CELL_TIMER,0x4c);
    {
        uint16_t selector=rd_u16(STREAM_SKIP);
        observe(hooks,CONTEXT_REFRESH_CONDITION_CALL,selector,0,0,0,0,0);
        hooks->consume(hooks->context,selector&1u ? CONTEXT_REFRESH_CONDITION_B : CONTEXT_REFRESH_CONDITION_A);
        wr_u16(CELL_TIMER,selector&1u?0x4e:0x4d);
    }
    observe(hooks,CONTEXT_REFRESH_RESTORE,0,0,0,0,0,0);
    if(!rd_u8(VIEW_MODE) && rd_u8(FIXED_READOUTS)) {
        uint16_t colour=rd_u8(REFRESH_PREPARED)?15:6;
        if(colour==15) wr_u32(LINE_STYLE,0x000fffffu);
        wr_u16(CURRENT_COLOUR,colour);
        observe(hooks,CONTEXT_REFRESH_RENDER_CALL,colour,0,0,0,0,0);
        hooks->consume(hooks->context,CONTEXT_REFRESH_RENDER);
    }
    wr_u8(REFRESH_PREPARED,0);
    observe(hooks,CONTEXT_REFRESH_DONE,0,0,0,0,0,0);
}
