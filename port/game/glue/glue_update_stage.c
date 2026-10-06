#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "update_stage.h"
#include "globals.h"
#include "glue_flight_record_calls.h"
#include "recomp_ports.h"
#include <stdlib.h>

typedef struct { int origin; } UpdateStageCPU;
static void stage_outputs(void *context,const UpdateStageEvent *event) {
    UpdateStageCPU *cpu=context;
    switch(event->phase) {
    case UPDATE_STAGE_PREPARED:
        D(5)=event->requests; D(0)=0u-event->value; renderer_negate(&D(0),4);
        SET_W(D(0),0); step_swap(&D(0)); step_asr_long(&D(0),5);
        if((uint16_t)event->coarse!=(uint16_t)event->previous) flags_logic_w(D(0));
        else step_compare_word(event->previous,D(0));
        break;
    case UPDATE_STAGE_RECORD_ROUTE:
        cpu->origin=0; A(3)=event->record; flags_logic_b(0); break;
    case UPDATE_STAGE_ORIGIN_SAVE:
        cpu->origin=1; A(7)-=2; m68k_write_memory_16(A(7),D(5)); flags_logic_w(D(5)); break;
    case UPDATE_STAGE_ORIGIN_RATE:
        A(3)=event->record; break;
    case UPDATE_STAGE_RECORD_KEYS:
        SET_W(D(0),event->value&3u); step_subtract_word(&D(0),3); renderer_negate(&D(0),2);
        SET_W(D(1),event->previous&3u); step_subtract_word(&D(1),3); renderer_negate(&D(1),2);
        renderer_asl_word(&D(1),2); step_add_word(&D(1),D(0)); flags_logic_b(D(1)); break;
    case UPDATE_STAGE_ORIGIN_KEYS:
        D(0)=(event->value<<16)|(event->value>>16);
        D(1)=(event->previous<<16)|(event->previous>>16);
        renderer_asr_word(&D(0),4); renderer_asr_word(&D(1),4);
        SET_W(D(2),D(0)); SET_W(D(3),D(1));
        SET_W(D(0),D(0)&3u); SET_W(D(1),D(1)&3u);
        step_subtract_byte(&D(0),3); renderer_negate(&D(0),1);
        step_subtract_byte(&D(1),3); renderer_negate(&D(1),1);
        renderer_add_byte(&D(1),D(1)); renderer_add_byte(&D(1),D(1)); renderer_add_byte(&D(1),D(0));
        renderer_asr_word(&D(2),2); renderer_asr_word(&D(3),2);
        SET_W(D(0),D(2)); SET_W(D(4),D(3));
        SET_W(D(2),D(2)&3u); SET_W(D(3),D(3)&3u);
        step_subtract_byte(&D(2),3); renderer_negate(&D(2),1);
        step_subtract_byte(&D(3),3); renderer_negate(&D(3),1);
        renderer_add_byte(&D(3),D(3)); renderer_add_byte(&D(3),D(3)); renderer_add_byte(&D(3),D(2)); break;
    case UPDATE_STAGE_REQUEST_ALL: D(5)=0xffffffffu; break;
    case UPDATE_STAGE_DONE: flags_logic_b(event->value); break;
    }
}
static UpdateStageResult consume(void *context,enum UpdateStageChild child) {
    UpdateStageCPU *cpu=context;
    UpdateStageResult result;
    if(child==UPDATE_STAGE_RECORDS) glue_complete_child(0xc22c80u,0xc1c6bcu);
    else if(child==UPDATE_STAGE_ORIGIN) {
        glue_complete_child(0xc29042u,0xc1c71eu);
        SET_W(D(5),m68k_read_memory_16(A(7))); A(7)+=2; flags_logic_w(D(5));
    } else glue_complete_child(0xc1c7f6u,cpu->origin?0xc1c72au:0xc1c6d4u);
    result.record=A(3); result.requests=(uint8_t)D(5); return result;
}
int glue_C1C63E(void) {
    UpdateStageCPU cpu={0};
    UpdateStageHooks hooks={consume,stage_outputs,&cpu};
    run_record_update_stage(&hooks);
    return glue_return();
}

typedef struct {
    RecordUpdateStageFrame frame;
    NativeControlRecordsCall records;
    UpdateStageCPU cpu;
    enum { UPDATE_CHILD_IDLE,UPDATE_CHILD_ORIGINAL,UPDATE_CHILD_RECORDS,UPDATE_CHILD_FINISHED } active;
} RecordUpdateStageCall;
static int continue_record_update_stage(const void *arguments) {
    RecordUpdateStageCall *call=(RecordUpdateStageCall *)arguments;
    for(;;) {
        if(call->active==UPDATE_CHILD_RECORDS) {
            int result=glue_continue_control_records(&call->records);
            if(result!=FA18_RET) return result;
            call->active=UPDATE_CHILD_FINISHED;
            fa18_ports_native_child_wait(REG_PC,A(7)); return FA18_EXIT_DISPATCH;
        }
        if(call->active!=UPDATE_CHILD_IDLE) {
            if(call->frame.child==UPDATE_STAGE_ORIGIN) {
                SET_W(D(5),rd_u16(A(7))); A(7)+=2; flags_logic_w(D(5));
            }
            call->frame.result=(UpdateStageResult){A(3),(uint8_t)D(5)};
            call->active=UPDATE_CHILD_IDLE;
        }
        UpdateStageHooks hooks={NULL,stage_outputs,&call->cpu};
        if(advance_record_update_stage(&call->frame,&hooks)) return glue_return();
        uint32_t entry,ret,sp=A(7);
        switch(call->frame.child) {
        case UPDATE_STAGE_RECORDS: entry=0xc22c80u; ret=0xc1c6bcu; break;
        case UPDATE_STAGE_ORIGIN: entry=0xc29042u; ret=0xc1c71eu; break;
        case UPDATE_STAGE_RATE: entry=0xc1c7f6u; ret=call->cpu.origin?0xc1c72au:0xc1c6d4u; break;
        default: abort();
        }
        m68ki_push_32(ret); REG_PC=entry;
        if(call->frame.child==UPDATE_STAGE_RECORDS) {
            fa18_ports_note_native_edge(0xc1c63e,0xc22c80);
            glue_begin_control_records(&call->records);
            call->active=UPDATE_CHILD_RECORDS; continue;
        }
        call->active=UPDATE_CHILD_ORIGINAL;
        fa18_ports_native_child_wait(ret,sp); return FA18_EXIT_DISPATCH;
    }
}
int glue_schedule_record_update_stage(void) {
    RecordUpdateStageCall call={0};
    return fa18_ports_schedule_native_child(continue_record_update_stage,&call,sizeof call,0);
}
