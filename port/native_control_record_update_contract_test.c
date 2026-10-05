#include "native_control_record_update.h"

#include "native_record_control_test_support.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput commands; FA18NativeSceneRecords records;
    FA18NativeControlRecordUpdate update; FA18NativeControlRecordOps ops;
    FA18NativeRecordSelection selection; FA18NativeRecordActionOps action_ops;
    FA18NativeRecordControl control_player; FA18RecordControlTestStorage control_storage;
    FA18NativeRecordRange range;
    uint8_t range_redraw; uint16_t range_magnitude;
    FA18NativeRecordView record_view; FA18NativeRecordViewWork view_work;
    FA18NativeRecordViewAssets view_assets;
    uint8_t view_mode,view_limit,pending,view_flag,created,admitted;
    uint16_t error_word; int16_t normalized[3];
    uint8_t source[16*512],work[16*32],event,first,second,primary,secondary;
    uint8_t selection_active,origin_enable,action_first,action_second,action_third,pair_override;
    uint16_t selected,selection_marker,action_pending;
    uint16_t periodic,slot,stride;
    unsigned calls[6],action_calls[3],dispatch_mask,pose_mask;
    FA18NativeControlRecordChild fail;
} Fixture;

static int consume(void *context,FA18NativeControlRecordUpdate *state,
                   FA18NativeControlRecordChild child,unsigned slot,unsigned companion,int *decision) {
    Fixture *f=context;
    assert(state==&f->update && decision && child<6 && slot<16);
    assert(companion<16);
    ++f->calls[child];
    if(child==f->fail) return 0;
    if(child==FA18_RECORD_UPDATE_DISPATCH) {
        f->dispatch_mask|=1u<<slot; *decision=1;
    } else if(child==FA18_RECORD_UPDATE_POSE) f->pose_mask|=1u<<slot;
    return 1;
}
static int action(void *context,FA18NativeRecordSelection *state,FA18NativeRecordActionChild child) {
    Fixture *f=context; assert(state==&f->selection && child<3); ++f->action_calls[child]; return 1;
}
static void put16(FA18NativeSceneRecord *record,size_t offset,uint16_t value) {
    uint8_t bytes[2]={(uint8_t)(value>>8),(uint8_t)value};
    assert(fa18_write_native_scene_record(record,offset,bytes,2));
}
static void initialize(Fixture *f) {
    memset(f,0,sizeof *f); f->fail=(FA18NativeControlRecordChild)99;
    assert(fa18_import_native_scene_records(&f->records,&f->commands,
        f->source,sizeof f->source,f->work,sizeof f->work));
    f->ops=(FA18NativeControlRecordOps){consume,f};
    f->action_ops=(FA18NativeRecordActionOps){action,f};
    f->selection=(FA18NativeRecordSelection){.records=&f->records,.ops=&f->action_ops,
        .selected_record=&f->selected,.selection_marker=&f->selection_marker,
        .action_pending=&f->action_pending,.selection_active=&f->selection_active,
        .origin_enable=&f->origin_enable,.action_first=&f->action_first,
        .action_second=&f->action_second,.action_third=&f->action_third,
        .pair_override=&f->pair_override};
    f->range=(FA18NativeRecordRange){.records=&f->records,.selected_record=&f->selected,
        .current_stride=&f->stride,.magnitude=&f->range_magnitude,.bar_redraw_f=&f->range_redraw};
    f->record_view=(FA18NativeRecordView){.records=&f->records,.assets=&f->view_assets,
        .selected_record=&f->selected,.current_stride=&f->stride,.current_slot=&f->slot,
        .tick_word=&f->periodic,.error_word=&f->error_word,.post_input_event=&f->event,
        .mode=&f->view_mode,.limit=&f->view_limit,.pending=&f->pending,.view_flag=&f->view_flag,
        .created=&f->created,.admitted=&f->admitted,.normalized=f->normalized};
    f->view_work.viewer=f->records.records;
    fa18_test_bind_record_control(&f->control_player,&f->control_storage,&f->records,
        &f->view_work,&f->slot,&f->event,&f->view_mode);
    f->update=(FA18NativeControlRecordUpdate){.records=&f->records,.selection=&f->selection,.range=&f->range,.ops=&f->ops,
        .view=&f->record_view,.view_work=&f->view_work,.control=&f->control_player,
        .post_input_event=&f->event,.counter_first=&f->first,.counter_second=&f->second,
        .primary_gate=&f->primary,.secondary_gate=&f->secondary,.periodic_word=&f->periodic,
        .current_slot=&f->slot,.current_stride=&f->stride};
}
int main(void) {
    static Fixture f; unsigned i; uint16_t expected_dispatch;
    initialize(&f); f.periodic=3; f.first=2; f.second=0x7f; f.secondary=1;
    f.records.records[0].byte_7c=3; f.pair_override=1;
    put16(f.records.records,0x4c,5); f.records.aircraft[0].flags=0xffff;
    for(i=0;i<16;++i) f.records.aircraft[i].secondary_flags=1;
    for(i=0;i<16;++i) if(i==3 || i==4 || i==6 || i==7 || i==8 || i==12 || i==13 || i==14 || i==15)
        f.records.aircraft[i].flags|=0x40;
    assert(fa18_update_native_control_records(&f.update));
    for(i=0;i<16;++i) assert(f.records.work[i][4]==0xff && f.records.work[i][5]==0xff);
    for(i=0;i<15;++i) assert(!(f.records.aircraft[i].secondary_flags&1));
    assert(f.records.aircraft[15].secondary_flags==1 && f.first==1 && f.second==0x7e);
    assert(f.calls[FA18_RECORD_UPDATE_PERIODIC]==1);
    assert(f.calls[FA18_RECORD_UPDATE_FINISH]==1);
    assert(f.calls[FA18_RECORD_UPDATE_SECONDARY_PLACE]==2);
    assert(f.action_calls[FA18_RECORD_ACTION_MANOEUVRE]==2 && !f.pair_override);
    expected_dispatch=(uint16_t)((1u<<1)|(1u<<2)|(1u<<3)|(1u<<4)|(1u<<5)|(1u<<6)|(1u<<8)|
        (1u<<9)|(1u<<12)|(1u<<13)|(1u<<14)|(1u<<15));
    assert(f.dispatch_mask==expected_dispatch && f.pose_mask==(expected_dispatch|1u));
    assert(!(f.dispatch_mask&(1u<<7)) && f.slot==15 && f.stride==15*512 &&
           f.update.companion_slot==12);
    assert((f.records.aircraft[14].flags&4) && (f.records.aircraft[15].flags&4));
    { uint8_t bytes[2]; assert(fa18_read_native_scene_record(f.records.records,0x4c,bytes,2));
      assert(!bytes[0] && bytes[1]==4); }

    initialize(&f); f.periodic=3; f.fail=FA18_RECORD_UPDATE_PERIODIC;
    assert(!fa18_update_native_control_records(&f.update));
    assert(f.records.work[0][4]==0xff && f.records.aircraft[0].secondary_flags==0);

    /* Slot four now uses the actual shared stream player, before dispatch. */
    initialize(&f); f.view_mode=125; f.records.records[0].word_6e=1;
    f.records.aircraft[4].flags=0x40;
    { uint8_t mode=3; assert(fa18_write_native_scene_record(f.records.records+4,5,&mode,1)); }
    assert(fa18_update_native_control_records(&f.update));
    { uint8_t mode; assert(fa18_read_native_scene_record(f.records.records+4,5,&mode,1)); assert(!mode); }
    assert(f.records.aircraft[4].secondary_flags==0x100 && (f.dispatch_mask&(1u<<4)));

    initialize(&f); f.update.control=NULL;
    assert(!fa18_update_native_control_records(&f.update) && !f.records.work[0][4]);
    initialize(&f); f.control_player.view_work=NULL;
    assert(!fa18_update_native_control_records(&f.update) && !f.records.work[0][4]);
    initialize(&f); f.control_player.mode=&f.primary;
    assert(!fa18_update_native_control_records(&f.update) && !f.records.work[0][4]);
    return 0;
}
