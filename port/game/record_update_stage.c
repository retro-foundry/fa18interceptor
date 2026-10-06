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
/* The slot cursor follows the original groups, including preparation-only
 * slot 7 and standalone records which retain the previous companion. */
static int advance_record_slot(ControlRecordsFrame *f,const RecordUpdateHooks *h) {
    unsigned slot=f->slot;
    int paired=slot==9 || slot==11 || slot==13;
    int group=slot<4 || slot==5 || paired;
    switch(f->slot_phase) {
    case CONTROL_SLOT_BEGIN:
        if(slot==7) { prepare_slot(h,7,6); active(h,7); return 1; }
        if(group) {
            prepare_slot(h,slot,slot<4?0:slot==5?4:slot-1);
            if(!active(h,slot)) {
                if(paired) f->child=RECORD_UPDATE_PAIRED_READY;
                else {
                    uint8_t mode=rd_u8(slot<4?0xc457bau:0xc457bbu);
                    observe(h,RECORD_UPDATE_GROUP_GATE,slot,mode,0,0,0);
                    if(mode) {
                        f->child=slot<4?RECORD_UPDATE_PRIMARY_PLACE:RECORD_UPDATE_SECONDARY_PLACE;
                        f->slot_phase=CONTROL_SLOT_AFTER_PLACE; return 0;
                    }
                    f->child=slot<4?RECORD_UPDATE_PRIMARY_READY:RECORD_UPDATE_SECONDARY_READY;
                }
                f->slot_phase=CONTROL_SLOT_AFTER_READY; return 0;
            }
        } else {
            if(!active(h,slot)) return 1;
            if(slot>=14) wr_u8(record_at(slot)+1,(uint8_t)(rd_u8(record_at(slot)+1)|4u));
            wr_u16(0xc459b4u,(uint16_t)slot); wr_u16(0xc459b6u,(uint16_t)(slot*512u));
            observe(h,RECORD_UPDATE_SLOT,slot,slot*512u,1,record_at(slot),0);
            if(slot==4) {
                f->child=RECORD_UPDATE_SECONDARY_CONTROL;
                f->slot_phase=CONTROL_SLOT_AFTER_CONTROL; return 0;
            }
        }
        break;
    case CONTROL_SLOT_AFTER_READY:
        if(!f->child_result) return 1;
        if(paired) {
            f->child=RECORD_UPDATE_SECONDARY_PLACE;
            f->slot_phase=CONTROL_SLOT_AFTER_PLACE; return 0;
        }
        break;
    case CONTROL_SLOT_AFTER_PLACE: case CONTROL_SLOT_AFTER_CONTROL: break;
    case CONTROL_SLOT_AFTER_DISPATCH:
        if(!f->child_result) return 1;
        f->child=RECORD_UPDATE_POSE; f->slot_phase=CONTROL_SLOT_AFTER_POSE; return 0;
    case CONTROL_SLOT_AFTER_POSE: return 1;
    default: abort();
    }
    f->child=RECORD_UPDATE_DISPATCH; f->slot_phase=CONTROL_SLOT_AFTER_DISPATCH; return 0;
}

int advance_control_records(ControlRecordsFrame *f,const RecordUpdateHooks *hooks) {
    unsigned slot;
    uint16_t periodic,root_countdown,root_header;
    uint8_t gate,first,second;
    if(!hooks) abort();
    switch(f->phase) {
    case CONTROL_RECORDS_BEGIN: break;
    case CONTROL_RECORDS_AFTER_PERIODIC: goto after_periodic;
    case CONTROL_RECORDS_AFTER_RELEASE: goto after_release;
    case CONTROL_RECORDS_AFTER_ROOT_CONTROL: goto after_root_control;
    case CONTROL_RECORDS_AFTER_ROOT_VIEW: goto after_root_view;
    case CONTROL_RECORDS_AFTER_ROOT_MARKER: goto after_root_marker;
    case CONTROL_RECORDS_AFTER_ROOT_POSE: goto after_root_pose;
    case CONTROL_RECORDS_SLOTS: goto slots;
    case CONTROL_RECORDS_AFTER_FINISH: goto after_finish;
    case CONTROL_RECORDS_COMPLETE: return 1;
    default: abort();
    }
    f->slot=0;
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
    if(periodic==3) { f->child=RECORD_UPDATE_PERIODIC; f->phase=CONTROL_RECORDS_AFTER_PERIODIC; return 0; }
after_periodic:
    /* The original clears bit zero in slots 0..14, leaving slot 15 alone. */
    for(slot=0;slot<15;++slot)
        wr_u16(record_at(slot)+2,(uint16_t)(rd_u16(record_at(slot)+2)&0xfffeu));
    gate=rd_u8(POST_INPUT_EVENT); first=rd_u8(0xc4584fu); second=rd_u8(0xc4584eu);
    if(!gate) {
        if((int8_t)first>0) wr_u8(0xc4584fu,(uint8_t)(first-1));
        if((int8_t)second>0) wr_u8(0xc4584eu,(uint8_t)(second-1));
    }
    observe(hooks,RECORD_UPDATE_RELEASE_GATE,0,gate,first|(uint32_t)second<<8,0,0);
    f->child=RECORD_UPDATE_RELEASE_SELECTION; f->phase=CONTROL_RECORDS_AFTER_RELEASE; return 0;
after_release:
    wr_u16(0xc459b4u,0); wr_u16(0xc459b6u,0);
    gate=rd_u8(POST_INPUT_EVENT); root_countdown=rd_u16(CONTROL_RECORDS+0x4cu);
    if(!gate) wr_u16(CONTROL_RECORDS+0x4cu,(uint16_t)(root_countdown-1));
    root_header=rd_u16(CONTROL_RECORDS)&0xfffdu; wr_u16(CONTROL_RECORDS,root_header);
    observe(hooks,RECORD_UPDATE_ROOT,0,root_header,gate?0x10000u:root_countdown,CONTROL_RECORDS,record_at(4));
    f->child=RECORD_UPDATE_ROOT_CONTROL; f->phase=CONTROL_RECORDS_AFTER_ROOT_CONTROL; return 0;
after_root_control:
    f->child=RECORD_UPDATE_ROOT_VIEW; f->phase=CONTROL_RECORDS_AFTER_ROOT_VIEW; return 0;
after_root_view:
    f->child=RECORD_UPDATE_ROOT_MARKER; f->phase=CONTROL_RECORDS_AFTER_ROOT_MARKER; return 0;
after_root_marker:
    f->child=RECORD_UPDATE_POSE; f->phase=CONTROL_RECORDS_AFTER_ROOT_POSE; return 0;
after_root_pose:
    f->slot=1; f->slot_phase=CONTROL_SLOT_BEGIN; f->phase=CONTROL_RECORDS_SLOTS;
slots:
    while(f->slot<16) {
        if(!advance_record_slot(f,hooks)) return 0;
        ++f->slot; f->slot_phase=CONTROL_SLOT_BEGIN;
    }
    f->slot=15; f->child=RECORD_UPDATE_FINISH; f->phase=CONTROL_RECORDS_AFTER_FINISH; return 0;
after_finish:
    observe(hooks,RECORD_UPDATE_RESTORE,0,0,0,0,0);
    f->phase=CONTROL_RECORDS_COMPLETE; return 1;
}
void update_control_records(const RecordUpdateHooks *hooks) {
    ControlRecordsFrame frame={0};
    if(!hooks || !hooks->consume) abort();
    while(!advance_control_records(&frame,hooks))
        frame.child_result=hooks->consume(hooks->context,frame.child,frame.slot);
}
