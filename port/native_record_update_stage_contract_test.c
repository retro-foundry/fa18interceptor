#include "native_record_update_stage.h"

#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput commands; FA18FlightCommandState flight; FA18ViewCommandState view;
    FA18NativeSceneRecords records; FA18NativeRecordUpdateStage update;
    FA18NativeRecordUpdateOps ops;
    uint8_t source[16*512],work[16*32];
    uint8_t input,mirror,inhibit,context_select,detail,coarse_byte,fine_byte,rate;
    int32_t bias,long_mirror,depth,origin[3];
    uint16_t scaled,word_x,word_z;
    uint8_t child_requests; unsigned record_calls,origin_calls;
} Fixture;

static int records(void *context,FA18NativeRecordUpdateStage *state,uint8_t *requests) {
    Fixture *f=context;
    assert(state==&f->update && requests && f->record_calls++==0);
    *requests=f->child_requests; return 1;
}
static int origin(void *context,FA18NativeRecordUpdateStage *state,int32_t output[3]) {
    Fixture *f=context;
    assert(state==&f->update && output==f->origin && f->origin_calls++==0);
    output[0]=0x10800000; output[1]=0x12345678; output[2]=0x11000000;
    return 1;
}
static void put16(FA18NativeSceneRecord *record,size_t offset,uint16_t value) {
    uint8_t bytes[2]={(uint8_t)(value>>8),(uint8_t)value};
    assert(fa18_write_native_scene_record(record,offset,bytes,2));
}
static void initialize(Fixture *f) {
    memset(f,0,sizeof *f);
    assert(fa18_import_native_scene_records(&f->records,&f->commands,
        f->source,sizeof f->source,f->work,sizeof f->work));
    f->flight.commands=&f->commands; f->flight.player=f->records.aircraft;
    f->flight.viewed=f->records.aircraft+2; f->view.flight=&f->flight;
    f->ops=(FA18NativeRecordUpdateOps){records,origin,f};
    f->update=(FA18NativeRecordUpdateStage){.records=&f->records,.view=&f->view,.ops=&f->ops,
        .input_byte=&f->input,.input_byte_mirror=&f->mirror,.change_inhibit=&f->inhibit,
        .context_selection=&f->context_select,.origin_detail_mode=&f->detail,
        .selector_byte_coarse=&f->coarse_byte,.selector_byte_fine=&f->fine_byte,.record_rate=&f->rate,
        .position_bias=&f->bias,.long_mirror=&f->long_mirror,.projection_depth=&f->depth,
        .origin=f->origin,.scaled_word=&f->scaled,.selector_word_x=&f->word_x,.selector_word_z=&f->word_z};
}
int main(void) {
    static Fixture f; FA18NativeSceneRecord *record;
    initialize(&f); record=f.records.records+2;
    f.input=7; f.bias=0x10000000; f.long_mirror=0; f.detail=2; f.child_requests=0x40;
    record->word_06=0x1235; record->word_08=0x4326; record->byte_0a=0x9a;
    put16(record,0x56,0x20); put16(record,0x58,0x40); put16(record,0x5a,0x100); record->word_6c=0x1001;
    assert(fa18_update_native_scene_records(&f.update));
    assert(f.mirror==7 && f.long_mirror==(int32_t)0xf0000000u && f.scaled==0x780);
    assert(f.record_calls==1 && !f.origin_calls && f.rate==3);
    assert(f.word_x==0x1235 && f.word_z==0x4326 && f.fine_byte==0x9a);
    assert(f.coarse_byte==6 && f.view.update_mask==0x40);

    initialize(&f); record=f.records.records;
    f.context_select=1; f.detail=2; f.depth=-0x1000; f.child_requests=0x0b;
    f.coarse_byte=f.fine_byte=1; f.word_x=f.word_z=1;
    put16(record,0x56,0xc1); put16(record,0x58,0); put16(record,0x5a,0);
    assert(fa18_update_native_scene_records(&f.update));
    assert(f.record_calls==1 && f.origin_calls==1 && f.rate==1);
    assert(f.origin[1]==0x12345678 && f.word_x==0x42 && f.word_z==0x44);
    assert(f.fine_byte==15 && f.coarse_byte==13 && f.view.update_mask==0xff);

    initialize(&f); f.update.ops=NULL;
    assert(!fa18_update_native_scene_records(&f.update));
    return 0;
}
