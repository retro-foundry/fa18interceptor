#include "native_record_view.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput input; FA18NativeSceneRecords records; FA18NativeRecordView view;
    FA18NativeRecordViewWork work; FA18NativeRecordViewAssets assets; FA18NativeRecordViewOps ops;
    uint8_t source[8192],workspace[512],parameters[32],status[256],list[32];
    uint16_t selected,stride,slot,tick,error;
    uint8_t event,mode,limit,pending,flag,created,admitted;
    int16_t normalized[3]; unsigned norms,faults; int fail;
} Fixture;
static int normalize(void *context,FA18NativeRecordView *state,int16_t scale,const int32_t vector[3],int16_t out[3]) {
    Fixture *f=context; assert(state==&f->view && scale==192 && vector);
    ++f->norms; out[0]=-192; out[1]=out[2]=0; return !f->fail;
}
static int fault(void *context,FA18NativeRecordView *state) {
    Fixture *f=context; assert(state==&f->view && (f->error==0x34 || f->error==0x35));
    ++f->faults; return !f->fail;
}
static void initialize(Fixture *f) {
    memset(f,0,sizeof *f);
    assert(fa18_import_native_scene_records(&f->records,&f->input,f->source,sizeof f->source,f->workspace,sizeof f->workspace));
    f->assets=(FA18NativeRecordViewAssets){
        .parameters={.bytes=f->parameters,.byte_count=sizeof f->parameters},
        .status={.bytes=f->status,.byte_count=sizeof f->status},
        .primary_list={.bytes=f->list,.byte_count=sizeof f->list}};
    f->ops=(FA18NativeRecordViewOps){normalize,fault,f};
    f->view=(FA18NativeRecordView){.records=&f->records,.assets=&f->assets,.ops=&f->ops,
        .selected_record=&f->selected,.current_stride=&f->stride,.current_slot=&f->slot,
        .tick_word=&f->tick,.error_word=&f->error,.post_input_event=&f->event,.mode=&f->mode,
        .limit=&f->limit,.pending=&f->pending,.view_flag=&f->flag,.created=&f->created,
        .admitted=&f->admitted,.normalized=f->normalized};
    f->work.viewer=f->records.records;
}
static void byte(Fixture *f,unsigned slot,size_t at,uint8_t v) {
    assert(fa18_write_native_scene_record(f->records.records+slot,at,&v,1));
}
static uint16_t word(Fixture *f,unsigned slot,size_t at) {
    uint8_t b[2]; assert(fa18_read_native_scene_record(f->records.records+slot,at,b,2));
    return (uint16_t)(((unsigned)b[0]<<8)|b[1]);
}
int main(void) {
    static Fixture f; FA18NativeSceneRecord *r;
    initialize(&f); f.event=1; f.pending=9; f.view.assets=NULL;
    /* Required owner binding still fails even on an event gate. */
    assert(!fa18_update_native_record_view(&f.view,0,&f.work));
    f.view.assets=&f.assets;
    assert(fa18_update_native_record_view(&f.view,0,&f.work) && f.pending==9);

    initialize(&f); r=f.records.records+1; byte(&f,1,0x38,0xff); byte(&f,1,0x7a,5);
    f.pending=7; f.records.geometry[0].inverse[2][2]=0x4000;
    assert(fa18_update_native_record_view(&f.view,1,&f.work));
    assert(!(r->aircraft->flags&1) && word(&f,1,0x32)==0x3f70 && f.pending==0);
    { uint8_t b; assert(fa18_read_native_scene_record(r,0x7a,&b,1) && b==5); }
    /* Indexed root runs the active gate and the preceding mode store. */
    initialize(&f); r=f.records.records+1; byte(&f,1,0x7a,5); f.pending=7;
    assert(fa18_update_native_record_view(&f.view,1,&f.work) && f.pending==7);
    { uint8_t b; assert(fa18_read_native_scene_record(r,0x7a,&b,1) && b==3); }

    initialize(&f); r=f.records.records+1; r->aircraft->flags=11; r->word_6c=0x4100;
    f.selected=0xffff;
    assert(fa18_update_native_record_view(&f.view,1,&f.work));
    assert(r->word_6c==0x4200 && r->word_6e==0x4200 && !(r->aircraft->flags&1));
    f.selected=2*512; f.records.records[2].word_06=0x1234;
    assert(fa18_update_native_record_view(&f.view,1,&f.work) && word(&f,1,0x2c)==0x1234);

    initialize(&f); r=f.records.records+1; r->aircraft->flags=0x1000;
    f.list[0]=f.list[1]=0xff; f.pending=7;
    assert(fa18_update_native_record_view(&f.view,1,&f.work));
    assert(f.error==0x34 && f.faults==1 && !f.pending);
    initialize(&f); r=f.records.records+1; r->aircraft->flags=0x1000;
    f.list[0]=f.list[1]=0xff; f.fail=1; f.pending=7;
    assert(!fa18_update_native_record_view(&f.view,1,&f.work));
    assert(f.error==0x34 && f.pending==7);

    initialize(&f); r=f.records.records+1; byte(&f,1,5,8); byte(&f,1,0x39,0x10);
    f.records.aircraft[1].secondary_flags=0x100; f.records.aircraft[1].flags=8;
    f.records.geometry[0].inverse[0][2]=f.records.geometry[1].inverse[0][2]=0x4000;
    f.mode=5; f.pending=1; f.status[0]=0x12; f.status[1]=0x34; f.status[3]=2;
    assert(fa18_update_native_record_view(&f.view,1,&f.work));
    assert(f.norms==1 && f.normalized[0]==-192 && (r->byte_04&0x20));
    assert(word(&f,1,0x4c)==0x1234 && f.created==1 && f.admitted==1 && !f.pending);
    assert(!(r->aircraft->flags&8));
    return 0;
}
