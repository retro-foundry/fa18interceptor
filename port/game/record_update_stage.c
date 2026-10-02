/* C22C80: source-ordered control-record update dispatch. */
#include "record_update_stage.h"
#include "globals.h"
#include <stdlib.h>

static gaddr record_at(unsigned slot) { return CONTROL_RECORDS+slot*512u; }
static void observe(const RecordUpdateHooks *hooks, enum RecordUpdatePhase phase,
                    unsigned slot, uint32_t value, uint32_t other,
                    gaddr record, gaddr companion) {
    RecordUpdateEvent event={phase,slot,value,other,record,companion};
    if(hooks->observe) hooks->observe(hooks->context,&event);
}
static void prepare_slot(const RecordUpdateHooks *hooks,unsigned slot,unsigned companion) {
    wr_u16(0xc459b4u,(uint16_t)slot);
    wr_u16(0xc459b6u,(uint16_t)(slot*512u));
    observe(hooks,RECORD_UPDATE_SLOT,slot,slot*512u,0,record_at(slot),record_at(companion));
}
static int active(const RecordUpdateHooks *hooks,unsigned slot) {
    uint8_t header=rd_u8(record_at(slot)+1);
    observe(hooks,RECORD_UPDATE_ACTIVE_BIT,slot,header,0,record_at(slot),0);
    return (header&0x40u)!=0;
}
static void dispatch_pose(const RecordUpdateHooks *hooks,unsigned slot) {
    if(hooks->consume(hooks->context,RECORD_UPDATE_DISPATCH,slot))
        hooks->consume(hooks->context,RECORD_UPDATE_POSE,slot);
}
static void update_group_record(const RecordUpdateHooks *hooks,unsigned slot,unsigned companion,
                                gaddr gate, enum RecordUpdateChild ready,
                                enum RecordUpdateChild place) {
    prepare_slot(hooks,slot,companion);
    if(!active(hooks,slot)) {
        uint8_t mode=rd_u8(gate);
        observe(hooks,RECORD_UPDATE_GROUP_GATE,slot,mode,0,0,0);
        if(mode) hooks->consume(hooks->context,place,slot);
        else if(!hooks->consume(hooks->context,ready,slot)) return;
    }
    dispatch_pose(hooks,slot);
}
static void update_active_record(const RecordUpdateHooks *hooks,unsigned slot,int force) {
    if(!active(hooks,slot)) return;
    if(force) wr_u8(record_at(slot)+1,(uint8_t)(rd_u8(record_at(slot)+1)|4u));
    wr_u16(0xc459b4u,(uint16_t)slot);
    wr_u16(0xc459b6u,(uint16_t)(slot*512u));
    /* Standalone records retain A2 from the preceding source stage. */
    observe(hooks,RECORD_UPDATE_SLOT,slot,slot*512u,1,record_at(slot),0);
    if(slot==4) hooks->consume(hooks->context,RECORD_UPDATE_SECONDARY_CONTROL,slot);
    dispatch_pose(hooks,slot);
}
static void update_paired_record(const RecordUpdateHooks *hooks,unsigned slot,unsigned companion) {
    prepare_slot(hooks,slot,companion);
    if(!active(hooks,slot)) {
        if(!hooks->consume(hooks->context,RECORD_UPDATE_PAIRED_READY,slot)) return;
        hooks->consume(hooks->context,RECORD_UPDATE_SECONDARY_PLACE,slot);
    }
    dispatch_pose(hooks,slot);
}
void update_control_records(const RecordUpdateHooks *hooks) {
    unsigned slot;
    uint16_t periodic,root_countdown,root_header;
    uint8_t gate,first,second;
    if(!hooks || !hooks->consume) abort();
    observe(hooks,RECORD_UPDATE_SAVE,0,0,0,0,0);
    if(!rd_u8(POST_INPUT_EVENT)) {
        uint16_t last=0;
        for(slot=0;slot<16;++slot) {
            gaddr counter=WORKSPACE_RECORDS+4+slot*32u;
            last=rd_u16(counter); wr_u16(counter,(uint16_t)(last-1));
        }
        observe(hooks,RECORD_UPDATE_COUNTDOWNS,0,last,0,0,0);
    }
    periodic=rd_u16(0xc458dau)&15u;
    observe(hooks,RECORD_UPDATE_PERIODIC_GATE,0,periodic,0,0,0);
    if(periodic==3) hooks->consume(hooks->context,RECORD_UPDATE_PERIODIC,0);
    /* The original clears bit zero in slots 0..14, leaving slot 15 alone. */
    for(slot=0;slot<15;++slot)
        wr_u16(record_at(slot)+2,(uint16_t)(rd_u16(record_at(slot)+2)&0xfffeu));
    gate=rd_u8(POST_INPUT_EVENT); first=rd_u8(0xc4584fu); second=rd_u8(0xc4584eu);
    if(!gate) {
        if((int8_t)first>0) wr_u8(0xc4584fu,(uint8_t)(first-1));
        if((int8_t)second>0) wr_u8(0xc4584eu,(uint8_t)(second-1));
    }
    observe(hooks,RECORD_UPDATE_RELEASE_GATE,0,gate,first|(uint32_t)second<<8,0,0);
    hooks->consume(hooks->context,RECORD_UPDATE_RELEASE_SELECTION,0);
    wr_u16(0xc459b4u,0); wr_u16(0xc459b6u,0);
    gate=rd_u8(POST_INPUT_EVENT); root_countdown=rd_u16(CONTROL_RECORDS+0x4cu);
    if(!gate) wr_u16(CONTROL_RECORDS+0x4cu,(uint16_t)(root_countdown-1));
    root_header=rd_u16(CONTROL_RECORDS)&0xfffdu; wr_u16(CONTROL_RECORDS,root_header);
    observe(hooks,RECORD_UPDATE_ROOT,0,root_header,gate?0x10000u:root_countdown,CONTROL_RECORDS,record_at(4));
    hooks->consume(hooks->context,RECORD_UPDATE_ROOT_CONTROL,0);
    hooks->consume(hooks->context,RECORD_UPDATE_ROOT_VIEW,0);
    hooks->consume(hooks->context,RECORD_UPDATE_ROOT_MARKER,0);
    hooks->consume(hooks->context,RECORD_UPDATE_POSE,0);
    for(slot=1;slot<4;++slot)
        update_group_record(hooks,slot,0,0xc457bau,RECORD_UPDATE_PRIMARY_READY,RECORD_UPDATE_PRIMARY_PLACE);
    update_active_record(hooks,4,0);
    update_group_record(hooks,5,4,0xc457bbu,RECORD_UPDATE_SECONDARY_READY,RECORD_UPDATE_SECONDARY_PLACE);
    update_active_record(hooks,6,0);
    prepare_slot(hooks,7,6);
    /* C22F14 tests slot 7, then C22F1A replaces A1 with slot 8 without
     * consuming that result or calling an update child for slot 7. */
    active(hooks,7);
    update_active_record(hooks,8,0);
    update_paired_record(hooks,9,8); update_active_record(hooks,10,0);
    update_paired_record(hooks,11,10); update_active_record(hooks,12,0);
    update_paired_record(hooks,13,12);
    update_active_record(hooks,14,1); update_active_record(hooks,15,1);
    hooks->consume(hooks->context,RECORD_UPDATE_FINISH,15);
    observe(hooks,RECORD_UPDATE_RESTORE,0,0,0,0,0);
}
