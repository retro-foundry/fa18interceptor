#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "record_update_stage.h"
#include "globals.h"
#include "glue_flight_record_calls.h"
#include "recomp_ports.h"

static void record_outputs(void *context,const RecordUpdateEvent *event) {
    uint32_t value;
    (void)context;
    switch(event->phase) {
    case RECORD_UPDATE_SAVE: m68ki_push_32(D(5)); flags_logic_l(D(5)); break;
    case RECORD_UPDATE_COUNTDOWNS:
        A(0)=WORKSPACE_RECORDS+4; value=event->value; step_subtract_word(&value,1); break;
    case RECORD_UPDATE_PERIODIC_GATE:
        SET_W(D(0),event->value); step_compare_word(3,D(0)); break;
    case RECORD_UPDATE_RELEASE_GATE:
        A(0)=CONTROL_RECORDS; SET_W(D(0),0xfffe); flags_logic_b(event->value);
        if(!event->value) {
            value=event->other&255u; flags_logic_b(value);
            if((int8_t)value>0) step_subtract_byte(&value,1);
            value=(event->other>>8)&255u; flags_logic_b(value);
            if((int8_t)value>0) step_subtract_byte(&value,1);
        }
        break;
    case RECORD_UPDATE_ROOT:
        A(1)=event->record; A(2)=event->companion;
        if(!(event->other&0x10000u)) { value=event->other; step_subtract_word(&value,1); }
        flags_logic_w(event->value); break;
    case RECORD_UPDATE_SLOT:
        A(1)=event->record;
        if(!event->other) A(2)=event->companion;
        flags_logic_w(event->value); break;
    case RECORD_UPDATE_ACTIVE_BIT:
        A(1)=event->record; FLAG_Z=event->value&0x40u; break;
    case RECORD_UPDATE_GROUP_GATE: flags_logic_b(event->value); break;
    case RECORD_UPDATE_RESTORE: D(5)=m68ki_pull_32(); flags_logic_l(D(5)); break;
    }
}
typedef struct { uint32_t entry,ret; } RecordUpdateCallSite;
static RecordUpdateCallSite child_site(enum RecordUpdateChild child,unsigned slot) {
    static const uint32_t pose_return[16]={
        0xc22d8e,0xc22dd4,0xc22e18,0xc22e5c,0xc22e8a,0xc22ece,0xc22ef8,0,
        0xc22f44,0xc22f7e,0xc22fa8,0xc22fe2,0xc2300c,0xc23046,0xc23076,0xc230a6};
    static const uint32_t dispatch_return[16]={
        0,0xc22dcc,0xc22e10,0xc22e54,0xc22e82,0xc22ec6,0xc22ef0,0,
        0xc22f3c,0xc22f76,0xc22fa0,0xc22fda,0xc23004,0xc2303e,0xc2306e,0xc2309e};
    static const uint32_t paired_return[3]={0xc22f6c,0xc22fd0,0xc23034};
    static const uint32_t place_return[3]={0xc22f72,0xc22fd6,0xc2303a};
    uint32_t routine=0,ret=0;
    switch(child) {
    case RECORD_UPDATE_PERIODIC: routine=0xc28996; ret=0xc22ce4; break;
    case RECORD_UPDATE_RELEASE_SELECTION: routine=0xc230b0; ret=0xc22d52; break;
    case RECORD_UPDATE_ROOT_CONTROL: routine=0xc23228; ret=0xc22d80; break;
    case RECORD_UPDATE_ROOT_VIEW: routine=0xc23ca6; ret=0xc22d84; break;
    case RECORD_UPDATE_ROOT_MARKER: routine=0xc244e2; ret=0xc22d88; break;
    case RECORD_UPDATE_POSE: routine=0xc25b66; ret=pose_return[slot]; break;
    case RECORD_UPDATE_PRIMARY_READY: routine=0xc230e8; ret=0xc22dc0+(slot-1)*0x44u; break;
    case RECORD_UPDATE_PRIMARY_PLACE: routine=0xc2374c; ret=0xc22dc8+(slot-1)*0x44u; break;
    case RECORD_UPDATE_SECONDARY_READY: routine=0xc23116; ret=0xc22eba; break;
    case RECORD_UPDATE_SECONDARY_PLACE:
        routine=0xc2377e; ret=slot==5?0xc22ec2:place_return[(slot-9)/2]; break;
    case RECORD_UPDATE_SECONDARY_CONTROL: routine=0xc233aa; ret=0xc22e7e; break;
    case RECORD_UPDATE_PAIRED_READY: routine=0xc231a2; ret=paired_return[(slot-9)/2]; break;
    case RECORD_UPDATE_DISPATCH: routine=0xc23a7e; ret=dispatch_return[slot]; break;
    case RECORD_UPDATE_FINISH: routine=0xc09e06; ret=0xc230ac; break;
    }
    return (RecordUpdateCallSite){routine,ret};
}
static int consume(void *context,enum RecordUpdateChild child,unsigned slot) {
    RecordUpdateCallSite site=child_site(child,slot); (void)context;
    glue_complete_child(site.entry,site.ret); return !COND_EQ();
}
int glue_C22C80(void) {
    RecordUpdateHooks hooks={consume,record_outputs,0};
    update_control_records(&hooks);
    return glue_return();
}

/* This parent composes record dynamics directly. Remaining children retain
 * their original runtime boundary until their C owners are connected here. */
int glue_continue_control_records(const void *arguments) {
    NativeControlRecordsCall *call=(NativeControlRecordsCall *)arguments;
    for(;;) {
        if(call->active==RECORD_CHILD_DYNAMICS) {
            int result=glue_continue_record_dynamics(&call->dynamics);
            if(result!=FA18_RET) return result;
            call->active=RECORD_CHILD_FINISHED;
            fa18_ports_native_child_wait(REG_PC,A(7)); return FA18_EXIT_DISPATCH;
        }
        if(call->active!=RECORD_CHILD_IDLE) {
            call->frame.child_result=!COND_EQ(); call->active=RECORD_CHILD_IDLE;
        }
        RecordUpdateHooks hooks={NULL,record_outputs,NULL};
        if(advance_control_records(&call->frame,&hooks)) return glue_return();
        RecordUpdateCallSite site=child_site(call->frame.child,call->frame.slot);
        uint32_t sp=A(7); m68ki_push_32(site.ret); REG_PC=site.entry;
        if(call->frame.child==RECORD_UPDATE_POSE) {
            fa18_ports_note_native_edge(0xc22c80,0xc25b66);
            glue_begin_record_dynamics(&call->dynamics);
            call->active=RECORD_CHILD_DYNAMICS; continue;
        }
        call->active=RECORD_CHILD_ORIGINAL;
        fa18_ports_native_child_wait(site.ret,sp); return FA18_EXIT_DISPATCH;
    }
}
void glue_begin_control_records(NativeControlRecordsCall *call) {
    *call=(NativeControlRecordsCall){0};
}
int glue_schedule_control_records(void) {
    NativeControlRecordsCall call; glue_begin_control_records(&call);
    return fa18_ports_schedule_native_child(glue_continue_control_records,&call,sizeof call,0);
}
