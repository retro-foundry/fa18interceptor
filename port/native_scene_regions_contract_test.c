#include "native_scene_regions.h"
#include "run075_trig_asset.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput input; FA18NativeSceneRecords records;
    FA18NativeRecordViewWork work;
    FA18NativeSceneRegions state; FA18NativeSceneRegionAssets assets;
    FA18NativeScenePointerGroup groups[16],source;
    FA18NativeRegionDescriptor descriptors[2];
    PortFieldWindow directory[12],parameters;
    uint8_t regions[11][20],table[64],model[8];
    uint8_t recorder,occupied,mode,admitted,active,reuse;
    uint16_t random,jitter_x,jitter_z;
} Fixture;
static void word(uint8_t *data,unsigned at,uint16_t value) {
    data[at]=(uint8_t)(value>>8); data[at+1]=(uint8_t)value;
}
static uint16_t record_word(const FA18NativeSceneRecord *r,unsigned at) {
    uint8_t bytes[2]; assert(fa18_read_native_scene_record(r,at,bytes,2));
    return (uint16_t)(((unsigned)bytes[0]<<8)|bytes[1]);
}
static void initialize(Fixture *f) {
    uint8_t records[8192]={0},work[512]={0}; unsigned i;
    memset(f,0,sizeof *f);
    assert(fa18_import_native_scene_records(&f->records,&f->input,records,sizeof records,work,sizeof work));
    f->model[6]=0xad; f->source.procedure=FA18_SCENE_PROCEDURE_RECORD_STREAM;
    f->source.data[0].data=(PortFieldWindow){.bytes=f->model,.byte_count=sizeof f->model};
    f->source.data[1].carried_value=0x81234567; f->source.data[1].carried_value_bound=1;
    f->descriptors[0]=(FA18NativeRegionDescriptor){.selector=0,.group=&f->source};
    f->descriptors[1]=(FA18NativeRegionDescriptor){.selector=0x140,.group=f->groups};
    word(f->table,0,16); word(f->table,2,32);
    word(f->table,16,0xffff); word(f->table,18,2); word(f->table,20,0x8000);
    word(f->table,22,0x7fff); word(f->table,24,3);
    word(f->table,32,4); word(f->table,34,5); word(f->table,36,6);
    word(f->table,38,7); word(f->table,40,8);
    f->parameters=(PortFieldWindow){.bytes=f->table,.byte_count=sizeof f->table};
    for(i=0;i<11;++i) {
        word(f->regions[i],0,0xffff); word(f->regions[i],2,1);
        word(f->regions[i],4,0xffff); word(f->regions[i],6,1);
        word(f->regions[i],8,1); word(f->regions[i],12,0x10);
        word(f->regions[i],14,1);
        f->directory[i]=(PortFieldWindow){.bytes=f->regions[i],.byte_count=20};
    }
    f->assets=(FA18NativeSceneRegionAssets){.parameters=&f->parameters,.directory=f->directory,
        .directory_count=12,.descriptors=f->descriptors,.descriptor_count=2,
        .trig={.bytes=fa18_run075_trig_bytes,.byte_count=sizeof fa18_run075_trig_bytes}};
    f->state=(FA18NativeSceneRegions){.records=&f->records,.view_work=&f->work,.assets=&f->assets,
        .pointer_groups=f->groups,.pointer_group_count=16,
        .recorder_on=&f->recorder,.occupied=&f->occupied,.mode=&f->mode,.admitted=&f->admitted,
        .active=&f->active,.reuse_jitter=&f->reuse,.random_word=&f->random,
        .jitter_x=&f->jitter_x,.jitter_z=&f->jitter_z};
    f->mode=3; f->admitted=16;
}
int main(void) {
    static Fixture f; uint32_t index; PortFieldWindow rows;
    initialize(&f); rows=f.directory[0]; rows.origin=10; index=7;
    assert(fa18_dispatch_native_region_records(&f.state,&rows,0,&index));
    assert(!index && f.active==1 && f.groups[1].procedure==FA18_SCENE_PROCEDURE_RECORD_STREAM);
    assert(f.groups[1].data[1].carried_value==0x81234567 && !f.work.carried_axis);
    assert(f.records.records[1].word_06==0xffff && f.records.records[1].byte_7d==13);
    assert(f.records.geometry[1].position[0]==0xff7fffc0 && f.records.geometry[1].position[1]==768);
    assert(record_word(f.records.records+1,0x30)==0x8000 && record_word(f.records.records+1,0x32)==0x7fff);
    assert(f.records.aircraft[1].flags==0x11c0 && f.records.records[1].forward[0][0]==0x4000);
    assert(f.records.records[1].geometry->inverse[2][2]==0x4000);
    /* Descriptor publication precedes signed admission rejection. The bank
     * can itself be the source, with the numeric operand copied live. */
    f.records.aircraft[1].flags=0; word(f.regions[0],10,0x140); f.groups[0]=f.source;
    f.groups[0].data[1].carried_value=0xfedcba98; f.admitted=f.active;
    assert(fa18_dispatch_native_region_records(&f.state,&rows,0,&index));
    assert(index==UINT32_MAX && f.work.carried_axis==0xfedcba0d);
    assert(f.groups[1].data[1].carried_value==0xfedcba98);
    /* Missing numeric metadata is explicit; no reference is converted to a
     * host pointer or silently replaced by its unrelocated asset offset. */
    f.groups[0].data[1].carried_value_bound=0;
    assert(!fa18_dispatch_native_region_records(&f.state,&rows,0,&index));
    initialize(&f); f.recorder=1; f.work.carried_axis=0x76543210;
    assert(fa18_update_native_scene_regions(&f.state) && !f.occupied && f.work.carried_axis==0x76543210);
    f.recorder=0; f.mode=2;
    assert(fa18_update_native_scene_regions(&f.state) && f.occupied==255 && f.work.carried_axis==1);
    /* Empty spawning resets the source index. Scanning can consequently
     * visit more than eight directory entries before its explicit sentinel. */
    initialize(&f);
    { unsigned i; for(i=0;i<11;++i) word(f.regions[i],8,0); }
    f.directory[10].byte_count=6;
    assert(!fa18_update_native_scene_regions(&f.state) && (f.occupied&1));
    initialize(&f); f.occupied=1; word(f.regions[0],0,1);
    word(f.regions[0],14,0x0201); f.records.aircraft[2].flags=0;
    { uint8_t state=3; assert(fa18_write_native_scene_record(f.records.records+1,0x7a,&state,1)); }
    f.directory[1]=(PortFieldWindow){0};
    assert(fa18_update_native_scene_regions(&f.state));
    assert(record_word(f.records.records+1,0x32)==0x0200);
    { uint8_t state; assert(fa18_read_native_scene_record(f.records.records+1,0x7a,&state,1)); assert(state==5); }
    /* A bare $FFFF counter means 65536 rows, not an empty loop. Missing
     * later rows fail after the first actual record publication. */
    initialize(&f); rows=f.directory[0]; rows.origin=10; index=0;
    assert(!fa18_dispatch_native_region_records(&f.state,&rows,0xffff,&index));
    assert(f.active==1 && (f.records.aircraft[1].flags&0x40));
    return 0;
}
