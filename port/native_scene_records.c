#include "native_scene_records.h"
#include <string.h>

static void bind_byte(FA18NativeSceneRecord *r,unsigned offset,uint8_t *owner) {
    *owner=r->unported[offset]; r->unported[offset]=0;
    r->fields[offset]=(PortFieldByte){.byte=owner};
}
static void bind_word(FA18NativeSceneRecord *r,unsigned offset,uint16_t *owner) {
    *owner=(uint16_t)(((unsigned)r->unported[offset]<<8)|r->unported[offset+1]);
    r->fields[offset]=(PortFieldByte){.unsigned_word=owner,.shift=8};
    r->fields[offset+1]=(PortFieldByte){.unsigned_word=owner};
    memset(r->unported+offset,0,2);
}
static void bind_signed_word(FA18NativeSceneRecord *r,unsigned offset,int16_t *owner) {
    unsigned bits=((unsigned)r->unported[offset]<<8)|r->unported[offset+1];
    *owner=bits<0x8000u?(int16_t)bits:(int16_t)((int32_t)bits-0x10000);
    r->fields[offset]=(PortFieldByte){.word=owner,.shift=8};
    r->fields[offset+1]=(PortFieldByte){.word=owner};
    memset(r->unported+offset,0,2);
}
static void bind_long(FA18NativeSceneRecord *r,unsigned offset,uint32_t *owner) {
    unsigned i;
    *owner=0;
    for(i=0;i<4;++i) {
        *owner=(*owner<<8)|r->unported[offset+i];
        r->fields[offset+i]=(PortFieldByte){.longword=owner,.shift=24-8*i};
    }
    memset(r->unported+offset,0,4);
}
int fa18_import_native_scene_records(FA18NativeSceneRecords *s,FA18CommandInput *input,
                                      const uint8_t *source,size_t source_bytes,
                                      const uint8_t *work,size_t work_bytes) {
    unsigned slot,i,row,column;
    if(!s || !input || !source || !work || source_bytes<16u*512 || work_bytes<16u*32) return 0;
    memset(s,0,sizeof *s); s->input=input;
    for(slot=0;slot<FA18_NATIVE_SCENE_RECORDS;++slot) {
        FA18NativeSceneRecord *r=s->records+slot;
        r->aircraft=s->aircraft+slot; r->geometry=s->geometry+slot;
        r->geometry->command_record=r->aircraft;
        memcpy(r->unported,source+slot*512,512);
        for(i=0;i<164;++i) r->fields[i]=(PortFieldByte){.byte=r->unported+i};
        bind_word(r,0,&r->aircraft->flags); bind_word(r,2,&r->aircraft->secondary_flags);
        for(i=0;i<3;++i) bind_long(r,0x14+4*i,r->geometry->position+i);
        bind_byte(r,0x21,&r->byte_21);
        r->level=slot?&r->level_storage:&input->indexed.control_record_level;
        bind_byte(r,0x2b,r->level);
        bind_long(r,0x3e,&r->long_3e); bind_long(r,0x42,&r->long_42); bind_long(r,0x46,&r->long_46);
        bind_long(r,0x50,&r->long_50); bind_word(r,0x54,&r->word_54);
        bind_long(r,0x56,&r->long_56); bind_word(r,0x5a,&r->word_5a);
        bind_byte(r,0x5f,&r->byte_5f); bind_word(r,0x60,&r->word_60);
        bind_byte(r,0x62,&r->aircraft->equipment_kind); bind_byte(r,0x63,&r->aircraft->weapon_radar);
        bind_byte(r,0x65,&r->aircraft->stick);
        bind_word(r,0x66,&r->angle_first); bind_word(r,0x68,&r->geometry->angle); bind_word(r,0x6a,&r->angle_third);
        bind_word(r,0x6c,&r->word_6c); bind_word(r,0x6e,&r->word_6e);
        bind_byte(r,0x71,&r->byte_71); bind_long(r,0x72,&r->long_72);
        bind_word(r,0x78,&r->word_78); bind_word(r,0x7e,&r->word_7e);
        for(row=0;row<3;++row) for(column=0;column<3;++column) {
            bind_signed_word(r,0x80+6*row+2*column,&r->forward[row][column]);
            bind_signed_word(r,0x92+6*row+2*column,&r->geometry->inverse[row][column]);
        }
    }
    memcpy(s->work,work,sizeof s->work); return 1;
}
int fa18_read_native_scene_record(const FA18NativeSceneRecord *r,size_t offset,
                                    uint8_t *bytes,size_t count) {
    size_t i;
    if(!r || !bytes || offset>512 || count>512-offset) return 0;
    for(i=0;i<count;++i) {
        size_t at=offset+i;
        if(at<164) { if(!port_read_field_byte(r->fields+at,bytes+i)) return 0; }
        else bytes[i]=r->unported[at];
    }
    return 1;
}
int fa18_write_native_scene_record(FA18NativeSceneRecord *r,size_t offset,
                                     const uint8_t *bytes,size_t count) {
    size_t i;
    if(!r || !bytes || offset>512 || count>512-offset) return 0;
    for(i=0;i<count;++i) {
        size_t at=offset+i;
        if(at<164) { if(!port_write_field_byte(r->fields+at,bytes[i])) return 0; }
        else r->unported[at]=bytes[i];
    }
    return 1;
}
int fa18_clear_native_scene_record(FA18NativeSceneRecord *r) {
    return r && r->aircraft && r->geometry && port_fill_field_bytes(r->fields,164,0);
}
int fa18_clear_native_bootstrap_records(FA18NativeSceneRecords *s) {
    unsigned slot;
    if(!s || !s->input) return 0;
    for(slot=0;slot<16;++slot) if(!fa18_clear_native_scene_record(s->records+slot)) return 0;
    for(slot=0;slot<16;++slot) memset(s->work[slot],0,32);
    return 1;
}
