#include "native_control_record_update.h"

#include "native_record_control_test_support.h"
#include "native_record_pose_test_support.h"
#include "native_record_action_placement_test_support.h"
#include "native_postflight_test_support.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput commands; FA18NativeSceneRecords records;
    FA18NativeControlRecordUpdate update; FA18NativeControlRecordOps ops;
    FA18NativeRecordSelection selection; FA18NativeRecordActionOps action_ops;
    FA18NativeRecordPose pose; FA18RecordPoseTestStorage pose_storage;
    FA18NativeRecordControl control_player; FA18RecordControlTestStorage control_storage;
    FA18NativeRecordActionPlacement action_placement; FA18RecordActionPlacementTestStorage placement_storage;
    FA18NativePostflight postflight; FA18PostflightTestStorage post_storage;
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
    unsigned calls[2],action_calls[3],dispatch_mask;
    FA18NativeControlRecordChild fail;
} Fixture;

static int consume(void *context,FA18NativeControlRecordUpdate *state,
                   FA18NativeControlRecordChild child,unsigned slot,unsigned companion,int *decision) {
    Fixture *f=context;
    assert(state==&f->update && decision && child<2 && slot<16);
    assert(companion<16);
    ++f->calls[child];
    if(child==f->fail) return 0;
    if(child==FA18_RECORD_UPDATE_DISPATCH) {
        f->dispatch_mask|=1u<<slot; *decision=1;
    }
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
    f->control_player.origin_enable=&f->origin_enable;
    fa18_test_bind_record_pose(&f->pose,&f->pose_storage,&f->control_player,&f->stride);
    fa18_test_bind_record_action_placement(&f->action_placement,&f->placement_storage,
        &f->control_player,&f->pose,&f->view_limit);
    f->update=(FA18NativeControlRecordUpdate){.records=&f->records,.selection=&f->selection,.range=&f->range,.ops=&f->ops,
        .view=&f->record_view,.view_work=&f->view_work,.control=&f->control_player,.pose=&f->pose,.placement=&f->action_placement,
        .post_input_event=&f->event,.counter_first=&f->first,.counter_second=&f->second,
        .primary_gate=&f->primary,.secondary_gate=&f->secondary,.periodic_word=&f->periodic,
        .current_slot=&f->slot,.current_stride=&f->stride};
    fa18_test_bind_postflight(&f->postflight,&f->post_storage,&f->update);
}
int main(void) {
    static Fixture f; unsigned i; uint16_t expected_dispatch;
    initialize(&f); f.periodic=3; f.first=2; f.second=0x7f; f.secondary=1;
    f.pair_override=1;
    put16(f.records.records,0x4c,5); f.records.aircraft[0].flags=0xffff;
    /* Root pose advances action two to three before the primary selectors. */
    for(i=0;i<16;++i) { f.records.aircraft[i].secondary_flags=1; f.records.records[i].byte_7c=i?1:2; }
    for(i=0;i<16;++i) if(i==3 || i==4 || i==6 || i==7 || i==8 || i==12 || i==13 || i==14 || i==15)
        f.records.aircraft[i].flags|=0x40;
    assert(fa18_update_native_control_records(&f.update));
    for(i=0;i<16;++i) assert(f.records.work[i][4]==0xff && f.records.work[i][5]==0xff);
    for(i=0;i<15;++i) assert(!(f.records.aircraft[i].secondary_flags&1));
    assert(f.records.aircraft[15].secondary_flags==1 && f.first==1 && f.second==0x7e);
    assert(f.calls[FA18_RECORD_UPDATE_PERIODIC]==1);
    assert(!f.placement_storage.space && !f.post_storage.report && !f.pair_override);
    assert(!(f.records.aircraft[5].flags&0x40) && !(f.records.aircraft[9].flags&0x40));
    assert(f.action_calls[FA18_RECORD_ACTION_MANOEUVRE]==2 && !f.pair_override);
    expected_dispatch=(uint16_t)((1u<<1)|(1u<<2)|(1u<<3)|(1u<<4)|(1u<<5)|(1u<<6)|(1u<<8)|
        (1u<<9)|(1u<<12)|(1u<<13)|(1u<<14)|(1u<<15));
    assert(f.dispatch_mask==expected_dispatch);
    for(i=0;i<16;++i) assert(f.records.records[i].byte_7c==(!i?3:(expected_dispatch&(1u<<i))?2:1));
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
    initialize(&f); f.update.pose=NULL;
    assert(!fa18_update_native_control_records(&f.update) && !f.records.work[0][4]);
    initialize(&f); f.pose.origin_enable=&f.primary;
    assert(!fa18_update_native_control_records(&f.update) && !f.records.work[0][4]);
    initialize(&f); f.update.placement=NULL;
    assert(!fa18_update_native_control_records(&f.update) && !f.records.work[0][4]);
    initialize(&f); f.action_placement.view_work=NULL;
    assert(!fa18_update_native_control_records(&f.update) && !f.records.work[0][4]);

    /* Actual primary stores are consumed in scheduler order, on the shared
     * root and descriptor bank, before the dispatch child observes them. */
    initialize(&f); f.event=1; f.primary=1; f.records.records[0].byte_5f=0x30;
    f.records.aircraft[0].weapon_radar=0x20;
    assert(fa18_update_native_control_records(&f.update));
    assert(!f.records.records[0].byte_5f && f.placement_storage.primary==3);
    for(i=1;i<4;++i) {
        assert(f.records.records[i].byte_5f==(uint8_t)(0x40-i*0x10));
        assert(f.placement_storage.groups[i].procedure==FA18_SCENE_PROCEDURE_RECORD_STREAM);
        assert(f.records.aircraft[i].flags==0x11c2);
    }
    assert(f.placement_storage.pending==1 && f.control_storage.stream_view==0xfb);

    initialize(&f); f.event=1; f.secondary=1; f.placement_storage.enable=1;
    f.records.aircraft[4].flags=0x40; f.records.aircraft[4].weapon_radar=0x30;
    f.records.records[4].byte_5f=2;
    assert(fa18_update_native_control_records(&f.update));
    assert(f.records.records[4].byte_5f==1 && f.records.records[5].byte_5f==2);
    assert(f.records.aircraft[5].equipment_kind==1 && f.records.aircraft[5].flags==0x11c2);
    assert(f.placement_storage.groups[5].procedure && !f.placement_storage.pending);

    /* Finish executes the actual mode-three owner after the native record
     * updates; it needs no outer scheduler or preparation child. */
    initialize(&f); f.event=1; f.view_mode=3; f.commands.indexed.cockpit_low_byte=0x40;
    f.post_storage.flags_f=0x80; f.control_storage.phase=2;
    f.placement_storage.space=3; f.post_storage.report=4;
    assert(fa18_update_native_control_records(&f.update));
    assert(f.post_storage.phase==0xff && f.control_storage.phase==3 && !f.post_storage.phase_word);
    assert(f.post_storage.step==4 && f.post_storage.context_gate==1 && !f.dispatch_mask);
    assert(!f.placement_storage.space && !f.post_storage.report);
    initialize(&f); f.update.postflight=NULL;
    assert(!fa18_update_native_control_records(&f.update) && !f.records.work[0][4]);
    return 0;
}
