#include "viewed_record_word.h"

static int view_word_read(void *context,uint16_t *value) {
    FA18NativeViewedRecordWord *s=context; unsigned i;
    if(!s || !s->records || !s->flight || !value) return 0;
    for(i=0;i<FA18_NATIVE_SCENE_RECORDS;++i)
        if(s->flight->viewed==s->records->aircraft+i) { *value=(uint16_t)(512*i); return 1; }
    return 0;
}
static int view_word_write(void *context,uint16_t value) {
    FA18NativeViewedRecordWord *s=context;
    if(!s || !s->records || !s->flight || value%512 || value/512>=FA18_NATIVE_SCENE_RECORDS) return 0;
    s->flight->viewed=s->records->aircraft+value/512;
    return 1;
}
int fa18_bind_native_viewed_record_word(FA18NativeViewedRecordWord *s,
                                         FA18NativeSceneRecords *records,
                                         FA18FlightCommandState *flight,
                                         PortFieldByte *words,size_t count) {
    uint8_t high,low; uint16_t value;
    if(!s || !records || !records->input || !flight || flight->commands!=records->input ||
       !words || count!=FA18_STARTUP_WORD_BYTES ||
       !port_read_field_byte(words+0x1e,&high) || !port_read_field_byte(words+0x1f,&low)) return 0;
    value=(uint16_t)(((unsigned)high<<8)|low);
    if(value%512 || value/512>=FA18_NATIVE_SCENE_RECORDS) return 0;
    *s=(FA18NativeViewedRecordWord){records,flight,{view_word_read,view_word_write,s}};
    if(!view_word_write(s,value)) return 0;
    words[0x1e]=(PortFieldByte){.word_value=&s->value,.shift=8};
    words[0x1f]=(PortFieldByte){.word_value=&s->value};
    return 1;
}
