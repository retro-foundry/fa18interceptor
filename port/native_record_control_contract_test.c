#include "native_record_control_test_support.h"
#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput input; FA18NativeSceneRecords records;
    FA18NativeRecordControl control; FA18RecordControlTestStorage storage;
    FA18NativeRecordViewWork work; FA18NativeRecordControlOps ops;
    FA18NativeControlStream streams[9];
    uint8_t source[8192],work_bytes[512],commands[512],event,mode;
    uint16_t slot,last_message;
    unsigned tones,messages,scenes;
    int complete;
} Fixture;
static int tone(void *context,FA18NativeRecordControl *state,unsigned program) {
    Fixture *f=context; assert(state==&f->control && program==8);
    ++f->tones; return f->complete;
}
static int message(void *context,FA18NativeRecordControl *state,uint16_t code) {
    Fixture *f=context; assert(state==&f->control);
    ++f->messages; f->last_message=code; return f->complete;
}
static int scene(void *context,FA18NativeRecordControl *state,unsigned slot,uint32_t *axis) {
    Fixture *f=context; assert(state==&f->control && slot==f->slot);
    assert(!f->records.aircraft[slot].flags);
    ++f->scenes; f->records.records[slot].word_6c=0x1234; *axis=0x87654321;
    return f->complete;
}
static void initialize(Fixture *f) {
    unsigned i;
    memset(f,0,sizeof *f);
    assert(fa18_import_native_scene_records(&f->records,&f->input,
        f->source,sizeof f->source,f->work_bytes,sizeof f->work_bytes));
    f->work.viewer=f->records.records; f->work.carried_axis=0x12345678;
    fa18_test_bind_record_control(&f->control,&f->storage,&f->records,
        &f->work,&f->slot,&f->event,&f->mode);
    f->storage.assets.streams=f->streams; f->storage.assets.count=9;
    for(i=0;i<8;++i) {
        f->storage.messages[8*i]=1; f->storage.messages[8*i+1]=(uint8_t)i;
        f->storage.messages[8*i+2]=2; f->storage.messages[8*i+3]=(uint8_t)i;
    }
    f->ops=(FA18NativeRecordControlOps){tone,message,scene,f};
    f->control.ops=&f->ops; f->complete=1;
}
static uint8_t byte(Fixture *f,size_t offset) {
    uint8_t value; assert(fa18_read_native_scene_record(f->records.records,offset,&value,1));
    return value;
}
static void set_byte(Fixture *f,size_t offset,uint8_t value) {
    assert(fa18_write_native_scene_record(f->records.records,offset,&value,1));
}
int main(void) {
    static Fixture f; unsigned i;
    initialize(&f); f.control.view_work=NULL;
    assert(!fa18_update_native_record_control(&f.control,0,4) && !f.tones);
    initialize(&f); f.records.records[0].long_56=0x04000000; f.complete=0;
    assert(!fa18_update_native_record_control(&f.control,0,4));
    assert(f.tones==1 && f.storage.alert==1 && f.work.carried_axis==0x12345678);
    f.complete=1;
    assert(fa18_update_native_record_control(&f.control,0,4) && f.tones==1);
    f.records.records[0].long_56=0;
    assert(fa18_update_native_record_control(&f.control,0,4) && !f.storage.alert);

    initialize(&f); f.mode=125; f.storage.gate=0xff; f.storage.pending=0xff;
    f.records.aircraft[0].secondary_flags=0x800; f.records.aircraft[0].weapon_radar=0x7f;
    f.records.aircraft[4].secondary_flags=0x123; f.records.aircraft[4].weapon_radar=3;
    f.records.records[4].word_6e=0x6789; set_byte(&f,164,0x55);
    for(i=0;i<3;++i) f.records.geometry[4].inverse[i][i]=16;
    f.records.geometry[4].position[0]=0x04000000;
    f.records.geometry[4].position[1]=0x80000000;
    f.records.geometry[4].position[2]=0xff000000;
    assert(fa18_update_native_record_control(&f.control,0,4));
    assert(f.messages==1 && f.last_message==0x101 && f.storage.pending==0xfe);
    assert(f.records.aircraft[0].weapon_radar==0x7f && f.records.aircraft[0].equipment_kind==17);
    assert(f.records.aircraft[0].secondary_flags==0x23 && !byte(&f,5) && !byte(&f,0x5e));
    assert(f.records.geometry[0].position[0]==0x04000009 && f.records.geometry[0].position[1]==0x80000001);
    assert(f.records.geometry[0].position[2]==0xfeffff9f && f.work.carried_axis==0xfffe3fff);
    assert(f.records.records[0].long_10==0xff800000 && f.records.records[0].word_6e==0x6789);
    assert(byte(&f,164)==0x55 && f.storage.redraw==0xff);

    initialize(&f); f.records.aircraft[0].secondary_flags=0x100; f.storage.index=1;
    f.streams[1].kind=FA18_CONTROL_STREAM_CODE; f.streams[1].code=3;
    assert(fa18_update_native_record_stream(&f.control,0));
    assert(byte(&f,5)==3 && f.storage.position==1 && !f.messages);
    assert(fa18_update_native_record_stream(&f.control,0) && !f.messages);
    set_byte(&f,5,0); f.records.aircraft[0].stick=0xff; f.mode=5;
    assert(fa18_update_native_record_stream(&f.control,0));
    assert(f.records.aircraft[0].stick==0xc3 && f.storage.pending==0xff && f.last_message==0x102);

    initialize(&f); f.records.aircraft[0].secondary_flags=0x100; f.storage.position=508;
    f.streams[0]=(FA18NativeControlStream){.kind=FA18_CONTROL_STREAM_COMMANDS,
        .commands={.bytes=f.commands,.byte_count=sizeof f.commands,.origin=1}};
    f.commands[509]=7; f.complete=0;
    assert(!fa18_update_native_record_stream(&f.control,0));
    assert(f.storage.position==509 && f.records.aircraft[0].stick==7 && f.records.aircraft[0].secondary_flags==0x100);
    assert(f.last_message==29);
    f.complete=1; f.storage.position=508;
    assert(fa18_update_native_record_stream(&f.control,0) && !f.records.aircraft[0].secondary_flags);
    f.records.aircraft[0].secondary_flags=0x100; f.storage.position=0xffff; f.commands[0]=9;
    assert(fa18_update_native_record_stream(&f.control,0));
    assert(f.storage.position==0 && f.records.aircraft[0].stick==9);

    initialize(&f); f.mode=2; f.storage.enable=1; f.storage.origin=1;
    f.records.aircraft[0].secondary_flags=0x900; f.records.aircraft[0].flags=0xffff;
    f.storage.metadata[2]=1; f.storage.metadata[3]=3;
    f.streams[1]=(FA18NativeControlStream){.kind=FA18_CONTROL_STREAM_COMMANDS,
        .commands={.bytes=f.commands,.byte_count=sizeof f.commands}};
    f.commands[0]=12;
    assert(fa18_update_native_record_control(&f.control,0,4));
    assert(f.scenes==1 && f.messages==1 && f.last_message==0x201);
    assert(f.records.records[0].word_6c==0x1234 && f.work.carried_axis==0x87654321);
    assert(f.storage.index==1 && f.storage.position==1 && f.storage.selector==3 && f.storage.origin==1);
    assert(f.records.aircraft[0].stick==12 && f.records.aircraft[0].secondary_flags==0x100);
    return 0;
}
