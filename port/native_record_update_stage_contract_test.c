#include "native_record_update_stage.h"

#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput commands; FA18FlightCommandState flight; FA18ViewCommandState view;
    FA18NativeSceneRecords records; FA18NativeRecordUpdateStage update;
    FA18NativeSelectorOrigin selector; FA18NativeSelectorOriginOps selector_ops;
    FA18NativeSelectorOriginTables selector_tables; FA18NativeSceneRecord *active;
    FA18NativeControlRecordUpdate control;
    FA18NativeRecordSelection selection;
    FA18NativeControlRecordOps control_ops;
    FA18NativeRecordRange range;
    uint8_t range_redraw; uint16_t range_magnitude;
    uint8_t source[16*512],work[16*32],selector_table[8];
    uint8_t input,mirror,inhibit,context_select,detail,coarse_byte,fine_byte,rate;
    int32_t bias,long_mirror,depth,origin[3];
    int32_t candidate[3],smoothed[3],negated[3],root_preset[3],aux_delta;
    uint16_t scaled,word_x,word_z;
    uint16_t angle_history,status;
    uint8_t origin_enable,gate_b,gate_a,gate_mode,origin_index,adjustment_mode;
    uint8_t threshold,auxiliary,variant,counter;
    uint8_t event,counter_a,counter_b,primary,secondary;
    uint8_t selection_active,action_first,action_second,action_third,pair_override;
    uint16_t periodic,current_slot,current_stride;
    uint16_t selected,selection_marker,action_pending;
    unsigned control_calls,origin_calls;
} Fixture;

static int control(void *context,FA18NativeControlRecordUpdate *state,
                   FA18NativeControlRecordChild child,unsigned slot,unsigned companion,int *decision) {
    Fixture *f=context;
    assert(state==&f->control && child<9 && slot<16 && companion<16 && decision);
    ++f->control_calls; *decision=0; return 1;
}
static int origin(void *context,FA18NativeSelectorOrigin *state,
                  FA18NativeSelectorOriginChild child,const int32_t input[3],int32_t output[3]) {
    Fixture *f=context;
    assert(state==&f->selector && child==FA18_SELECTOR_ORIGIN_PREPARE && !input && !output);
    ++f->origin_calls;
    return 1;
}
static void put16(FA18NativeSceneRecord *record,size_t offset,uint16_t value) {
    uint8_t bytes[2]={(uint8_t)(value>>8),(uint8_t)value};
    assert(fa18_write_native_scene_record(record,offset,bytes,2));
}
static void initialize(Fixture *f) {
    PortFieldWindow window;
    memset(f,0,sizeof *f);
    assert(fa18_import_native_scene_records(&f->records,&f->commands,
        f->source,sizeof f->source,f->work,sizeof f->work));
    f->flight.commands=&f->commands; f->flight.player=f->records.aircraft;
    f->flight.viewed=f->records.aircraft+2; f->view.flight=&f->flight;
    f->control_ops=(FA18NativeControlRecordOps){control,f};
    f->selection=(FA18NativeRecordSelection){.records=&f->records,.selected_record=&f->selected,
        .selection_marker=&f->selection_marker,.action_pending=&f->action_pending,
        .selection_active=&f->selection_active,.origin_enable=&f->origin_enable,
        .action_first=&f->action_first,.action_second=&f->action_second,
        .action_third=&f->action_third,.pair_override=&f->pair_override};
    f->range=(FA18NativeRecordRange){.records=&f->records,.selected_record=&f->selected,
        .current_stride=&f->current_stride,.magnitude=&f->range_magnitude,.bar_redraw_f=&f->range_redraw};
    f->control=(FA18NativeControlRecordUpdate){.records=&f->records,.selection=&f->selection,.range=&f->range,.ops=&f->control_ops,
        .post_input_event=&f->event,.counter_first=&f->counter_a,.counter_second=&f->counter_b,
        .primary_gate=&f->primary,.secondary_gate=&f->secondary,.periodic_word=&f->periodic,
        .current_slot=&f->current_slot,.current_stride=&f->current_stride};
    f->active=f->records.records; f->selector_ops=(FA18NativeSelectorOriginOps){origin,f};
    window=(PortFieldWindow){.bytes=f->selector_table,.byte_count=sizeof f->selector_table};
    f->selector_tables=(FA18NativeSelectorOriginTables){window,window,window,window};
    f->selector=(FA18NativeSelectorOrigin){.records=&f->records,.active_record=&f->active,
        .ops=&f->selector_ops,.tables=&f->selector_tables,.root_preset=f->root_preset,
        .origin=f->origin,.candidate=f->candidate,.smoothed_delta=f->smoothed,
        .negated_companion=f->negated,.auxiliary_delta=&f->aux_delta,
        .angle_history=&f->angle_history,.status_word=&f->status,.enable=&f->origin_enable,
        .gate_b=&f->gate_b,.gate_a=&f->gate_a,.gate_mode=&f->gate_mode,.detail_mode=&f->detail,
        .detail_index=&f->origin_index,.adjustment_mode=&f->adjustment_mode,
        .threshold_flag=&f->threshold,.auxiliary_flag=&f->auxiliary,
        .variant_selector=&f->variant,.detail_counter=&f->counter};
    f->update=(FA18NativeRecordUpdateStage){.records=&f->records,.view=&f->view,
        .control_records=&f->control,.origin_update=&f->selector,
        .input_byte=&f->input,.input_byte_mirror=&f->mirror,.change_inhibit=&f->inhibit,
        .context_selection=&f->context_select,.origin_detail_mode=&f->detail,
        .selector_byte_coarse=&f->coarse_byte,.selector_byte_fine=&f->fine_byte,.record_rate=&f->rate,
        .position_bias=&f->bias,.long_mirror=&f->long_mirror,.projection_depth=&f->depth,
        .origin=f->origin,.scaled_word=&f->scaled,.selector_word_x=&f->word_x,.selector_word_z=&f->word_z};
}
int main(void) {
    static Fixture f; FA18NativeSceneRecord *record;
    initialize(&f); record=f.records.records+2;
    f.input=7; f.bias=0x10000000; f.long_mirror=0; f.detail=2;
    record->word_06=0x1235; record->word_08=0x4326; record->byte_0a=0x9a;
    put16(record,0x56,0x20); put16(record,0x58,0x40); put16(record,0x5a,0x100); record->word_6c=0x1001;
    assert(fa18_update_native_scene_records(&f.update));
    assert(f.mirror==7 && f.long_mirror==(int32_t)0xf0000000u && f.scaled==0x780);
    assert(f.control_calls && !f.origin_calls && f.rate==3);
    assert(f.word_x==0x1235 && f.word_z==0x4326 && f.fine_byte==0x9a);
    assert(f.coarse_byte==6 && f.view.update_mask==0x0b);

    initialize(&f); record=f.records.records;
    f.context_select=1; f.origin_enable=f.gate_b=f.gate_mode=1; f.depth=-0x1000;
    f.coarse_byte=f.fine_byte=1; f.word_x=f.word_z=1;
    record->geometry->position[0]=0x10800000; record->geometry->position[2]=0x11000000;
    f.origin[1]=0x12345678;
    put16(record,0x56,0xc1); put16(record,0x58,0); put16(record,0x5a,0);
    assert(fa18_update_native_scene_records(&f.update));
    assert(f.control_calls && f.origin_calls==1 && f.rate==1);
    assert(f.origin[1]==0x12345678 && f.word_x==0x42 && f.word_z==0x44);
    assert(f.fine_byte==15 && f.coarse_byte==13 && f.view.update_mask==0xff);

    initialize(&f); f.update.origin_update=NULL;
    assert(!fa18_update_native_scene_records(&f.update));
    return 0;
}
