#include "native_control_record_update.h"

#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput commands; FA18NativeSceneRecords records;
    FA18NativeControlRecordUpdate update; FA18NativeControlRecordOps ops;
    uint8_t source[16*512],work[16*32],event,first,second,primary,secondary;
    uint16_t periodic,slot,stride;
    unsigned calls[14],dispatch_mask,pose_mask;
    FA18NativeControlRecordChild fail;
} Fixture;

static int consume(void *context,FA18NativeControlRecordUpdate *state,
                   FA18NativeControlRecordChild child,unsigned slot,unsigned companion,int *decision) {
    Fixture *f=context;
    assert(state==&f->update && decision && child<14 && slot<16);
    assert(companion<16);
    ++f->calls[child];
    if(child==f->fail) return 0;
    if(child==FA18_RECORD_UPDATE_DISPATCH) {
        f->dispatch_mask|=1u<<slot; *decision=1;
    } else if(child==FA18_RECORD_UPDATE_POSE) f->pose_mask|=1u<<slot;
    else if(child==FA18_RECORD_UPDATE_PRIMARY_READY) *decision=slot!=2;
    else if(child==FA18_RECORD_UPDATE_SECONDARY_READY) *decision=1;
    else if(child==FA18_RECORD_UPDATE_PAIRED_READY) *decision=slot==9;
    return 1;
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
    f->update=(FA18NativeControlRecordUpdate){.records=&f->records,.ops=&f->ops,
        .post_input_event=&f->event,.counter_first=&f->first,.counter_second=&f->second,
        .primary_gate=&f->primary,.secondary_gate=&f->secondary,.periodic_word=&f->periodic,
        .current_slot=&f->slot,.current_stride=&f->stride};
}
int main(void) {
    static Fixture f; unsigned i; uint16_t expected_dispatch;
    initialize(&f); f.periodic=3; f.first=2; f.second=0x7f; f.secondary=1;
    put16(f.records.records,0x4c,5); f.records.aircraft[0].flags=0xffff;
    for(i=0;i<16;++i) f.records.aircraft[i].secondary_flags=1;
    for(i=0;i<16;++i) if(i==3 || i==4 || i==6 || i==7 || i==8 || i==12 || i==13 || i==14 || i==15)
        f.records.aircraft[i].flags|=0x40;
    assert(fa18_update_native_control_records(&f.update));
    for(i=0;i<16;++i) assert(f.records.work[i][4]==0xff && f.records.work[i][5]==0xff);
    for(i=0;i<15;++i) assert(!(f.records.aircraft[i].secondary_flags&1));
    assert(f.records.aircraft[15].secondary_flags==1 && f.first==1 && f.second==0x7e);
    assert(f.calls[FA18_RECORD_UPDATE_PERIODIC]==1 && f.calls[FA18_RECORD_UPDATE_RELEASE_SELECTION]==1);
    assert(f.calls[FA18_RECORD_UPDATE_ROOT_CONTROL]==1 && f.calls[FA18_RECORD_UPDATE_ROOT_VIEW]==1 &&
           f.calls[FA18_RECORD_UPDATE_ROOT_MARKER]==1 && f.calls[FA18_RECORD_UPDATE_FINISH]==1);
    assert(f.calls[FA18_RECORD_UPDATE_PRIMARY_READY]==2 && f.calls[FA18_RECORD_UPDATE_SECONDARY_PLACE]==2);
    assert(f.calls[FA18_RECORD_UPDATE_SECONDARY_CONTROL]==1 && f.calls[FA18_RECORD_UPDATE_PAIRED_READY]==2);
    expected_dispatch=(uint16_t)((1u<<1)|(1u<<3)|(1u<<4)|(1u<<5)|(1u<<6)|(1u<<8)|
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
    return 0;
}
