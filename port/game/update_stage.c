#include "update_stage.h"
#include "globals.h"
#include <stdlib.h>

static void observe(const UpdateStageHooks *hooks,enum UpdateStagePhase phase,
                    uint32_t value,uint32_t previous,uint32_t coarse,uint32_t flags,
                    gaddr record,uint8_t requests) {
    UpdateStageEvent event={phase,value,previous,coarse,flags,record,requests};
    if(hooks->observe) hooks->observe(hooks->context,&event);
}
static uint8_t reverse_cell(uint16_t x,uint16_t z) {
    return (uint8_t)((3u-(x&3u))+4u*(3u-(z&3u)));
}
static int origin_request_allowed(void) {
    return rd_u8(0xc458aeu)!=2 || rd_s32(0xc45a78u)>-0x1000;
}
void run_record_update_stage(const UpdateStageHooks *hooks) {
    uint8_t requests=0,current,previous;
    uint32_t negated,old_value,coarse;
    uint16_t old_coarse;
    UpdateStageResult child;
    if(!hooks || !hooks->consume) abort();
    current=rd_u8(0xc45854u); previous=rd_u8(0xc45855u);
    if(current!=previous) {
        wr_u8(0xc45855u,current);
        if(!rd_u8(0xc45786u)) requests|=0xbu;
    }
    negated=0u-rd_u32(POSITION_BIAS); old_value=rd_u32(0xc45a6eu);
    if(((int32_t)negated>=0xa000)!=((int32_t)old_value>=0xa000)) requests|=0xbu;
    wr_u32(0xc45a6eu,negated);
    /* CLR.W, SWAP, ASR.L #5 produces an unsigned high-word key. */
    coarse=(negated>>16)>>5; old_coarse=rd_u16(0xc45a5eu);
    if((uint16_t)coarse!=old_coarse) {
        if(rd_u8(0xc458aeu)!=2) requests|=0xbu;
        wr_u16(0xc45a5eu,(uint16_t)coarse);
    }
    observe(hooks,UPDATE_STAGE_PREPARED,negated,old_coarse,coarse,0,0,requests);
    child=hooks->consume(hooks->context,UPDATE_STAGE_RECORDS); requests=child.requests;
    if(!rd_u8(CONTEXT_SELECT)) {
        gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(VIEW_RECORD);
        uint16_t x,z;
        observe(hooks,UPDATE_STAGE_RECORD_ROUTE,0,0,0,0,record,requests);
        child=hooks->consume(hooks->context,UPDATE_STAGE_RATE); record=child.record;
        x=rd_u16(record+6); wr_u16(0xc4594cu,x);
        z=rd_u16(record+8); wr_u16(0xc4594eu,z);
        wr_u8(0xc45851u,rd_u8(record+0xau));
        wr_u8(0xc45850u,reverse_cell(x,z));
        observe(hooks,UPDATE_STAGE_RECORD_KEYS,x,z,0,0,record,requests);
    } else {
        uint32_t x,z;
        uint16_t coarse_x,coarse_z;
        uint8_t first,second;
        observe(hooks,UPDATE_STAGE_ORIGIN_SAVE,0,0,0,0,0,requests);
        child=hooks->consume(hooks->context,UPDATE_STAGE_ORIGIN); requests=child.requests;
        observe(hooks,UPDATE_STAGE_ORIGIN_RATE,0,0,0,0,CONTROL_RECORDS,requests);
        hooks->consume(hooks->context,UPDATE_STAGE_RATE);
        x=rd_u32(0xc45c3eu)&0x1fffffffu; z=rd_u32(0xc45c46u)&0x1fffffffu;
        coarse_x=(uint16_t)((x>>16)>>4); coarse_z=(uint16_t)((z>>16)>>4);
        first=reverse_cell(coarse_x,coarse_z);
        coarse_x=(uint16_t)(coarse_x>>2); coarse_z=(uint16_t)(coarse_z>>2);
        second=reverse_cell(coarse_x,coarse_z);
        observe(hooks,UPDATE_STAGE_ORIGIN_KEYS,x,z,0,0,0,requests);
        if(first!=rd_u8(0xc45851u)) {
            if(origin_request_allowed()) { requests=0xff; observe(hooks,UPDATE_STAGE_REQUEST_ALL,0,0,0,0,0,requests); }
            wr_u8(0xc45851u,first);
        }
        if(second!=rd_u8(0xc45850u)) {
            if(origin_request_allowed()) { requests=0xff; observe(hooks,UPDATE_STAGE_REQUEST_ALL,0,0,0,0,0,requests); }
            wr_u8(0xc45850u,second);
        }
        if(coarse_x!=rd_u16(0xc4594cu)) {
            requests=0xff; observe(hooks,UPDATE_STAGE_REQUEST_ALL,0,0,0,0,0,requests);
            wr_u16(0xc4594cu,coarse_x);
        }
        if(coarse_z!=rd_u16(0xc4594eu)) {
            requests=0xff; observe(hooks,UPDATE_STAGE_REQUEST_ALL,0,0,0,0,0,requests);
            wr_u16(0xc4594eu,coarse_z);
        }
    }
    wr_u8(UPDATE_MASK,(uint8_t)(rd_u8(UPDATE_MASK)|requests));
    observe(hooks,UPDATE_STAGE_DONE,rd_u8(UPDATE_MASK),0,0,0,0,requests);
}
