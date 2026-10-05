#include "native_record_range.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput input; FA18NativeSceneRecords records; FA18NativeRecordRange range;
    uint8_t source[8192],work[512],bytes[516],redraw;
    uint16_t selected,stride,magnitude;
    FA18NativeRecordRangeOps ops; PortFieldWindow table;
    unsigned tones; int fail;
} Fixture;
static int tone(void *context,FA18NativeRecordRange *s,unsigned program) {
    Fixture *f=context; assert(s==&f->range && program==4); ++f->tones;
    *s->selected_record=512; return !f->fail;
}
static void initialize(Fixture *f) {
    unsigned i; memset(f,0,sizeof *f);
    assert(fa18_import_native_scene_records(&f->records,&f->input,f->source,sizeof f->source,f->work,sizeof f->work));
    for(i=0;i<sizeof f->bytes;i+=2) f->bytes[i]=0x40;
    f->table=(PortFieldWindow){.bytes=f->bytes,.byte_count=sizeof f->bytes};
    f->ops=(FA18NativeRecordRangeOps){tone,f};
    f->range=(FA18NativeRecordRange){.records=&f->records,.table=&f->table,.ops=&f->ops,
        .selected_record=&f->selected,.current_stride=&f->stride,.magnitude=&f->magnitude,.bar_redraw_f=&f->redraw};
}
static uint16_t distance(Fixture *f) {
    uint8_t b[2]; assert(fa18_read_native_scene_record(f->records.records,0x4a,b,2));
    return (uint16_t)(((unsigned)b[0]<<8)|b[1]);
}
int main(void) {
    static Fixture f; uint8_t mode,bytes[2];
    initialize(&f); f.records.records[0].word_6c=0x1200;
    f.records.records[1].long_10=0x300; f.stride=512;
    f.records.aircraft[0].weapon_radar=5; mode=3;
    assert(fa18_write_native_scene_record(f.records.records,0x7a,&mode,1));
    assert(fa18_classify_native_selected_range(&f.range,0));
    assert(f.tones==1 && f.redraw==1 && f.selected==512 && f.magnitude==0x300 && distance(&f)==0x300);
    assert(f.records.aircraft[0].weapon_radar==0x15 && (f.records.records[0].byte_04&1));
    assert(fa18_read_native_scene_record(f.records.records,0x7a,&mode,1) && mode==4);
    f.records.records[1].long_10=0x10000;
    assert(fa18_classify_native_selected_range(&f.range,0));
    assert(distance(&f)==0x7fff && f.records.aircraft[0].weapon_radar==0xf5 && f.tones==1);
    initialize(&f); f.fail=1; f.records.records[0].word_6c=0x1200;
    assert(!fa18_classify_native_selected_range(&f.range,0) && f.redraw==1 && f.selected==512);
    initialize(&f); f.selected=1;
    assert(!fa18_classify_native_selected_range(&f.range,0));
    initialize(&f); f.selected=512; f.range.table=NULL;
    assert(!fa18_classify_native_selected_range(&f.range,0));
    assert(fa18_read_native_scene_record(f.records.records,0x39,bytes,1) && bytes[0]==0);
    return 0;
}
