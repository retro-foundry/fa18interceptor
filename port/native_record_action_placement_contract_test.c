#include "native_record_control_test_support.h"
#include "native_record_pose_test_support.h"
#include "native_record_action_placement_test_support.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput input; FA18NativeSceneRecords records;
    FA18NativeRecordControl control; FA18RecordControlTestStorage control_storage;
    FA18NativeRecordPose pose; FA18RecordPoseTestStorage pose_storage;
    FA18NativeRecordActionPlacement placement; FA18RecordActionPlacementTestStorage storage;
    FA18NativeRecordViewWork work;
    uint16_t slot,stride; uint8_t event,mode,limit,source[8192],workspace[512];
} Fixture;
static void offset(Fixture *f,int other,unsigned row,int16_t a,int16_t b,int16_t c) {
    int16_t v[3]={a,b,c}; unsigned i;
    size_t at=(other?600:512)+row*6;
    for(i=0;i<3;++i) {
        f->storage.offsets[at+2*i]=(uint8_t)((uint16_t)v[i]>>8);
        f->storage.offsets[at+2*i+1]=(uint8_t)v[i];
    }
}
static uint16_t word(FA18NativeSceneRecord *r,size_t at) {
    uint8_t b[2]; assert(fa18_read_native_scene_record(r,at,b,2));
    return (uint16_t)(((unsigned)b[0]<<8)|b[1]);
}
static void initialize(Fixture *f,unsigned slot) {
    unsigned i;
    memset(f,0,sizeof *f); memset(f->source,0x5a,sizeof f->source);
    assert(fa18_import_native_scene_records(&f->records,&f->input,
        f->source,sizeof f->source,f->workspace,sizeof f->workspace));
    f->slot=(uint16_t)slot;
    fa18_test_bind_record_control(&f->control,&f->control_storage,&f->records,
        &f->work,&f->slot,&f->event,&f->mode);
    fa18_test_bind_record_pose(&f->pose,&f->pose_storage,&f->control,&f->stride);
    fa18_test_bind_record_action_placement(&f->placement,&f->storage,&f->control,&f->pose,&f->limit);
    f->storage.assets.offsets_other.origin=600;
    f->storage.assets.primary.data[0].segment=17;
    f->storage.assets.alternate.data[0].segment=18;
    f->storage.primary=0xffff; f->storage.alternate=7;
    f->storage.warnings=0x80004001; f->pose_storage.events=0x10;
    f->storage.space=4; f->storage.pending=9; f->work.carried_axis=0x1234abcd;
    f->records.aircraft[0].flags=0x8080; f->records.aircraft[0].secondary_flags=0x84;
    f->records.aircraft[0].equipment_kind=0x10; f->records.aircraft[0].weapon_radar=0x21;
    f->records.records[0].byte_5f=0x35; f->records.records[0].long_42=0x11223344;
    for(i=0;i<3;++i) {
        memset(f->records.geometry[0].inverse[i],0,sizeof f->records.geometry[0].inverse[i]);
        f->records.geometry[0].inverse[i][i]=64;
        f->records.geometry[0].position[i]=i?100:0xfffffff0;
    }
    offset(f,0,2,17,-33,45); offset(f,1,2,-6,7,-8); offset(f,1,6,11,-12,13);
}
static void check_loader(void) {
    FA18HunkSegment segments[19]={0}; FA18Hunks hunks={0};
    FA18HunkReloc relocations[6]; FA18NativeRecordActionPlacementAssets assets;
    uint8_t source[0x1a80]={0},target[32]={0}; unsigned row,i;
    hunks.count=19; hunks.segments=segments;
    segments[16].data=source; segments[16].size=sizeof source;
    segments[16].relocs=relocations; segments[16].reloc_count=6;
    segments[17].data=source; segments[17].size=sizeof source;
    segments[18].data=target; segments[18].size=sizeof target;
    for(row=0;row<2;++row) for(i=0;i<3;++i) {
        unsigned at=row*20+4+4*i;
        source[at+3]=(uint8_t)(i+1);
        relocations[row*3+i].offset=at; relocations[row*3+i].target=18;
    }
    assert(fa18_load_native_record_action_placement_assets(&hunks,&assets));
    assert(assets.primary.procedure==FA18_SCENE_PROCEDURE_RECORD_STREAM);
    assert(assets.alternate.data[2].segment==18 && assets.alternate.data[2].offset==3);
    assert(assets.alternate.data[2].data.bytes==target && assets.alternate.data[2].data.origin==3);
    assert(!assets.alternate.data[3].data.bytes);
    assert(assets.offsets_10.bytes==source+0xdb2 && !assets.offsets_10.origin && assets.offsets_10.byte_count==0x4e);
    assert(assets.offsets_other.origin==0x24);
    { int16_t v; assert(port_field_window_s16(&assets.offsets_other,40,&v));
      assert(!port_field_window_s16(&assets.offsets_other,42,&v)); }
    source[7]=33;
    assert(!fa18_load_native_record_action_placement_assets(&hunks,&assets));
    assert(!fa18_load_native_scene_pointer_group(&hunks,40,&assets.primary));
}
int main(void) {
    static Fixture f; FA18NativeSceneRecord *r; unsigned i;
    check_loader(); initialize(&f,1); r=f.records.records+1;
    assert(fa18_place_native_primary_record(&f.placement,1,0));
    assert(f.storage.space==0 && f.storage.pending==1 && f.storage.warnings==0x80000001);
    assert(f.pose_storage.events==0x18 && f.storage.primary==0 && f.storage.alternate==7 && f.storage.changed==1);
    assert(f.storage.alert==8 && f.control_storage.stream_view==0xfb);
    assert(f.records.records[0].byte_5f==0x25 && r->byte_5f==0x35);
    assert(f.records.geometry[1].position[0]==1 && f.records.geometry[1].position[1]==67 &&
        f.records.geometry[1].position[2]==145 && f.work.carried_axis==0xffffffdf);
    assert(r->aircraft->equipment_kind==0 && r->aircraft->flags==0x91c2 && !r->aircraft->secondary_flags);
    assert(word(r,0x4c)==150 && word(r,0x26)==20 && word(r,0x2c)==0xffff && !word(r,0x76));
    assert(!r->long_42 && !r->long_56 && !r->word_5a && f.control_storage.redraw==0x8c);
    assert(f.storage.groups[1].data[0].segment==17 && f.storage.redraw_a==3 && f.storage.redraw_b==3);
    for(i=164;i<512;++i) { uint8_t b; assert(fa18_read_native_scene_record(r,i,&b,1) && b==0x5a); }

    /* Aliasing changes the companion kind before choosing its launch table. */
    initialize(&f,0);
    assert(fa18_place_native_primary_record(&f.placement,0,0));
    assert(f.records.geometry[0].position[0]==0xffffffea && f.records.geometry[0].position[1]==107);
    assert(f.records.records[0].byte_5f==0x25 && f.work.carried_axis==7);

    initialize(&f,1); f.records.aircraft[0].weapon_radar=0x30;
    f.records.aircraft[0].equipment_kind=0x12;
    assert(fa18_place_native_primary_record(&f.placement,1,0));
    assert(f.records.aircraft[1].equipment_kind==1 && f.records.records[0].byte_5f==0x34);
    assert(f.storage.alternate==8 && word(f.records.records+1,0x4c)==200);
    assert(f.storage.groups[1].data[0].segment==18 && f.work.carried_axis==0xfffffff4);

    initialize(&f,1); f.records.records[0].byte_5f=5;
    assert(fa18_place_native_primary_record(&f.placement,1,0));
    assert(!f.storage.groups[1].procedure && f.work.carried_axis==0x1234abcd && f.storage.pending==1);

    initialize(&f,1); f.storage.enable=1; f.limit=2; f.pose_storage.selector=4;
    assert(fa18_place_native_secondary_record(&f.placement,1,0));
    assert(!f.storage.groups[1].procedure && f.storage.space==4 && f.storage.pending==9);
    f.storage.divisor=1;
    assert(fa18_place_native_secondary_record(&f.placement,1,0));
    assert(f.storage.groups[1].procedure && f.storage.warnings==0x80004001 && f.pose_storage.events==0x10);

    initialize(&f,1); f.slot=16;
    assert(!fa18_place_native_primary_record(&f.placement,1,0));
    assert(f.storage.primary==0 && f.records.aircraft[1].equipment_kind==0 && f.records.records[0].byte_5f==0x35);
    initialize(&f,1); f.storage.assets.offsets_10.byte_count=0;
    assert(!fa18_place_native_primary_record(&f.placement,1,0));
    assert(f.records.records[0].byte_5f==0x25 && f.storage.redraw_a==3 && f.storage.groups[1].procedure);
    return 0;
}
