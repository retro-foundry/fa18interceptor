#include "native_record_control_test_support.h"
#include "native_record_pose_test_support.h"
#include "native_record_action_placement_test_support.h"
#include "native_postflight_test_support.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput input; FA18NativeSceneRecords records;
    FA18NativeRecordControl control; FA18RecordControlTestStorage control_storage;
    FA18NativeRecordPose pose; FA18RecordPoseTestStorage pose_storage;
    FA18NativeRecordActionPlacement placement; FA18RecordActionPlacementTestStorage placement_storage;
    FA18NativeRecordSelection selection; FA18NativeRecordView view; FA18NativeRecordViewAssets assets;
    FA18NativeRecordViewWork work; FA18NativeControlRecordUpdate update;
    FA18NativePostflight post; FA18PostflightTestStorage storage; FA18NativePostflightOps ops;
    uint8_t source[8192],workspace[512],table[1024],event,mode,limit,admitted,active,pair,actions[3];
    uint16_t slot,stride,selected,marker,pending;
    FA18FlightCommandState flight; FA18ViewCommandState commands_view;
    FA18ContextCommandState context; FA18CommandQueue queue;
    FA18ViewSpanOffsets spans; FA18NativeContextPublication publication;
    uint8_t neighbors[FA18_COMMAND_QUEUE_NEIGHBORS],keys[128]; uint16_t target;
    unsigned prepared; int complete;
} Fixture;
static int prepare(void *context,FA18NativePostflight *s,unsigned slot,uint16_t event,int seven) {
    Fixture *f=context;
    assert(s==&f->post && slot==4 && event==1 && !seven && !f->event);
    ++f->prepared;
    *s->sequence_phase=2; *s->current_slot=12; s->view_work->carried_axis=0xcafebabe;
    return f->complete;
}
static void put16(uint8_t *p,uint16_t n) { p[0]=(uint8_t)(n>>8); p[1]=(uint8_t)n; }
static uint16_t read_word(FA18NativeSceneRecord *r,size_t at) {
    uint8_t b[2]; assert(fa18_read_native_scene_record(r,at,b,2));
    return (uint16_t)(((unsigned)b[0]<<8)|b[1]);
}
static void initialize(Fixture *f) {
    memset(f,0,sizeof *f);
    assert(fa18_import_native_scene_records(&f->records,&f->input,f->source,sizeof f->source,
        f->workspace,sizeof f->workspace));
    fa18_test_bind_record_control(&f->control,&f->control_storage,&f->records,&f->work,&f->slot,&f->event,&f->mode);
    fa18_test_bind_record_pose(&f->pose,&f->pose_storage,&f->control,&f->stride);
    fa18_test_bind_record_action_placement(&f->placement,&f->placement_storage,&f->control,&f->pose,&f->limit);
    f->assets.parameters=(PortFieldWindow){.bytes=f->table,.byte_count=sizeof f->table,.origin=256};
    f->view=(FA18NativeRecordView){.assets=&f->assets,.mode=&f->mode,.limit=&f->limit,.admitted=&f->admitted};
    f->selection=(FA18NativeRecordSelection){.records=&f->records,.selected_record=&f->selected,
        .selection_marker=&f->marker,.action_pending=&f->pending,.selection_active=&f->active,
        .origin_enable=f->control.origin_enable,.action_first=f->actions,.action_second=f->actions+1,
        .action_third=f->actions+2,.pair_override=&f->pair};
    f->update=(FA18NativeControlRecordUpdate){.records=&f->records,.control=&f->control,.selection=&f->selection,
        .view=&f->view,.view_work=&f->work,.placement=&f->placement,.current_slot=&f->slot,.post_input_event=&f->event};
    fa18_test_bind_postflight(&f->post,&f->storage,&f->update);
    f->ops=(FA18NativePostflightOps){prepare,f}; f->post.ops=&f->ops; f->complete=1;
}
int main(void) {
    static Fixture f; FA18NativeSceneRecord *r; int ready; unsigned i;
    initialize(&f); f.input.indexed.cockpit_low_byte=0x40; f.mode=8;
    f.selected=5*512; f.marker=3; f.active=1; f.pair=1;
    f.placement_storage.space=4; f.storage.report=9; f.admitted=2; f.storage.aux=3; f.event=1;
    assert(fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_FINISH,0x1234));
    assert(f.selected==0xffff && f.marker==0xffff && !f.active && !f.pair);
    assert(!f.placement_storage.space && !f.storage.report && !f.event);
    assert(f.storage.phase_word==0x0a02 && f.storage.phase==0xff && f.control_storage.phase==3);
    assert(f.storage.step==4 && f.storage.context_gate==1);

    initialize(&f); f.storage.phase=1; r=f.records.records;
    r->byte_21=1; r->byte_04=4; r->aircraft->secondary_flags=0x80;
    assert(fa18_native_postflight_ready(&f.post,&ready) && ready);
    assert(fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_THREE,0xabcd));
    assert(f.storage.phase==0xfc && !f.storage.phase_word && f.control_storage.phase==3);
    f.input.indexed.pose_entry=3;
    assert(fa18_native_postflight_ready(&f.post,&ready) && !ready);
    r->byte_04=0x80; assert(fa18_native_postflight_ready(&f.post,&ready) && ready);

    initialize(&f); r=f.records.records+4; r->word_06=1;
    r->aircraft->flags=0x40; r->aircraft->secondary_flags=0x80; r->geometry->angle=0x1234;
    f.event=2; f.control_storage.selector=5;
    assert(fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_FOUR,0));
    assert(f.slot==4 && f.storage.saved_view==0x85 && f.control_storage.selector==7 && f.event==1);
    assert(f.storage.smooth==1 && f.storage.started==1 && f.storage.heading==0x1234);
    assert(f.storage.command_word==2 && f.storage.refresh==0xff && !f.storage.phase);
    assert(fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_FOUR,0));
    assert(f.prepared==1 && f.slot==12 && f.work.carried_axis==0xcafebabe);
    assert(f.storage.phase==0xff && f.storage.phase_word==4 && f.control_storage.phase==3);

    initialize(&f); r=f.records.records+4; r->word_06=1;
    r->aircraft->flags=0x40; r->aircraft->secondary_flags=0x80; f.event=1; f.complete=0;
    assert(!fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_FOUR,0));
    assert(f.prepared==1 && !f.event && !f.storage.phase && f.control_storage.phase==2);

    initialize(&f); r=f.records.records+7;
    { uint8_t index=7; assert(fa18_write_native_scene_record(r,0x3a,&index,1)); }
    put16(f.table+256+14,0xfff0);
    { static const uint16_t words[]={0xfffe,0x7fff,0x8000,4,0xfffa};
      for(i=0;i<5;++i) put16(f.table+240+2*i,words[i]); }
    assert(fa18_restore_native_postflight_view(&f.post,7));
    assert(read_word(r,0x2c)==0xfffe && read_word(r,0x30)==0x8000 && f.work.carried_axis==0xffff8000);
    { uint8_t bytes[4]; assert(fa18_read_native_scene_record(r,0x34,bytes,4));
      assert(bytes[0]==0xff && bytes[1]==0xff && bytes[2]==0xff && bytes[3]==0xfa); }

    initialize(&f); f.records.aircraft[4].flags=0x48;
    f.records.geometry[0].position[0]=0x7fffffff; f.records.geometry[4].position[0]=0x80000000;
    f.records.geometry[0].position[1]=0x80000000; f.records.geometry[4].position[1]=0x7fffffff;
    f.event=2;
    assert(fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_FIVE,0));
    assert(f.storage.gate==0xffff && f.work.carried_axis==0xffffffff && f.event==1 && !f.storage.phase);

    initialize(&f); f.records.aircraft[4].flags=0x48; f.storage.gate=0xffff; f.event=1;
    { uint8_t index=1; assert(fa18_write_native_scene_record(f.records.records+6,0x3a,&index,1)); }
    put16(f.table+256,64); put16(f.table+258,80);
    for(i=0;i<5;++i) { put16(f.table+320+2*i,(uint16_t)i); put16(f.table+336+2*i,(uint16_t)(8+i)); }
    assert(fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_FIVE,0));
    assert(read_word(f.records.records+4,0x30)==2 && read_word(f.records.records+6,0x30)==10);
    assert(f.work.carried_axis==10 && f.storage.phase==0xff && !f.storage.phase_word);

    initialize(&f); f.storage.target=1;
    assert(!fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_SIX,0));
    initialize(&f); f.post.phase_fields=NULL;
    assert(!fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_FINISH,0));
    /* Actual preparation, including a signed queue index reaching the live
     * sequence phase. Parent must reread it before publishing its outcome. */
    initialize(&f);
    f.flight.commands=&f.input; f.flight.player=f.records.aircraft;
    f.flight.viewed=f.records.aircraft;
    f.commands_view.flight=&f.flight;
    f.context=(FA18ContextCommandState){.view=&f.commands_view,.records=f.records.geometry,.record_count=16};
    f.keys[1]=3;
    assert(fa18_initialize_command_queue(&f.queue,&f.context,f.neighbors,sizeof f.neighbors,f.keys,sizeof f.keys));
    f.publication=(FA18NativeContextPublication){&f.records,&f.context,&f.queue,&f.spans,&f.marker,&f.target};
    f.post.publication=&f.publication; f.post.ops=NULL;
    f.post.context_select=&f.input.origin_mode; f.selection.origin_enable=&f.input.origin_mode;
    f.post.sequence_phase=&f.flight.sequence_phase;
    f.post.view_side=&f.commands_view.detail_index;
    f.post.refresh=&f.context.view_request; f.post.view_heading=&f.context.angle_history;
    r=f.records.records+4; r->word_06=1; r->aircraft->flags=0x40;
    r->aircraft->secondary_flags=0x80; r->geometry->angle=0x4567; f.event=1;
    f.queue.translated_index=63; f.work.carried_axis=0x12345678;
    assert(fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_FOUR,0));
    assert(!f.prepared && !f.storage.phase && f.flight.sequence_phase==3);
    assert(f.slot==4 && f.target==4 && f.flight.viewed==f.records.aircraft+4);
    assert(f.context.angle_history==0x4567 && f.work.carried_axis==63);
    assert(f.queue.raw[0]==1 && f.marker==0xffff && !f.event && !f.commands_view.emitted_requests);
    /* Missing canonical aliases fail instead of running a contracted child. */
    f.event=1; f.queue.taken=0; f.flight.sequence_phase=2;
    f.post.refresh=&f.storage.refresh;
    assert(!fa18_schedule_native_postflight(&f.post,FA18_POSTFLIGHT_FOUR,0));
    assert(!f.prepared && !f.event && f.flight.sequence_phase==2);
    f.records.input=NULL; f.flight.commands=NULL; f.queue.commands=NULL;
    { uint32_t axis=0x12345678,published=0x87654321;
      assert(!fa18_publish_native_context_record(&f.publication,1,4,&axis,&published));
      assert(axis==0x12345678 && published==0x87654321); }
    return 0;
}
