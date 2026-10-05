#include "native_record_selection.h"

#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput input; FA18NativeSceneRecords records;
    FA18NativeRecordSelection selection; FA18NativeRecordActionOps ops;
    uint8_t source[16*512],work[16*32];
    uint16_t selected,marker,pending;
    uint8_t active,origin_enable,first,second,third,override;
    unsigned calls[3]; FA18NativeRecordActionChild fail;
} Fixture;
static int consume(void *context,FA18NativeRecordSelection *state,FA18NativeRecordActionChild child) {
    Fixture *f=context; assert(state==&f->selection && child<3); ++f->calls[child];
    return child!=f->fail;
}
static void put8(FA18NativeSceneRecord *record,size_t offset,uint8_t value) {
    assert(fa18_write_native_scene_record(record,offset,&value,1));
}
static void initialize(Fixture *f) {
    memset(f,0,sizeof *f); f->fail=(FA18NativeRecordActionChild)99;
    assert(fa18_import_native_scene_records(&f->records,&f->input,
        f->source,sizeof f->source,f->work,sizeof f->work));
    f->ops=(FA18NativeRecordActionOps){consume,f};
    f->selection=(FA18NativeRecordSelection){.records=&f->records,.ops=&f->ops,
        .selected_record=&f->selected,.selection_marker=&f->marker,.action_pending=&f->pending,
        .selection_active=&f->active,.origin_enable=&f->origin_enable,.action_first=&f->first,
        .action_second=&f->second,.action_third=&f->third,.pair_override=&f->override};
}
int main(void) {
    static Fixture f; int decision;
    initialize(&f); f.selected=2*512; f.active=1; f.marker=2;
    assert(fa18_release_lost_native_selection(&f.selection));
    assert(f.selected==0xffff && !f.active && f.marker==0xffff);
    initialize(&f); f.selected=512; f.records.aircraft[1].flags=0x40;
    assert(fa18_release_lost_native_selection(&f.selection) && f.selected==512);

    initialize(&f); f.records.records[0].byte_7c=0xae;
    assert(fa18_select_native_record_action(&f.selection,1,&decision));
    assert(decision==1 && f.records.records[0].byte_7c==0xa0 && f.calls[FA18_RECORD_ACTION_RELEASE]==1);
    f.records.records[0].byte_7c=9;
    assert(fa18_select_native_record_action(&f.selection,0,&decision));
    assert(!decision && f.calls[FA18_RECORD_ACTION_SOUND]==1);
    f.records.records[0].byte_7c=1;
    assert(fa18_select_native_record_action(&f.selection,0,&decision));
    assert(!decision && f.first==0x3f && f.second==0x78 && f.third==0xff);
    f.records.records[0].byte_7c=3;
    assert(fa18_select_native_record_action(&f.selection,0,&decision));
    assert(decision && f.calls[FA18_RECORD_ACTION_MANOEUVRE]==1);

    initialize(&f); f.override=7;
    assert(fa18_native_paired_record_ready(&f.selection,8,&decision));
    assert(decision && !f.override);
    initialize(&f); f.records.aircraft[8].flags=0x41; f.records.aircraft[8].weapon_radar=0x2f;
    put8(f.records.records+8,0x38,0); put8(f.records.records+8,0x64,0x60);
    assert(fa18_native_paired_record_ready(&f.selection,8,&decision) && decision);
    initialize(&f); f.records.aircraft[8].flags=0x41; f.records.aircraft[8].weapon_radar=0x20;
    put8(f.records.records+8,0x38,0x80); put8(f.records.records+8,0x64,0x60);
    f.records.aircraft[0].flags=0x40;
    assert(fa18_native_paired_record_ready(&f.selection,8,&decision) && decision);
    f.records.aircraft[0].flags|=1;
    assert(fa18_native_paired_record_ready(&f.selection,8,&decision) && !decision);
    return 0;
}
