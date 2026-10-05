#include "startup_ranges.h"

int fa18_bind_native_startup_ranges(FA18NativeStartupRanges *s,FA18CommandQueue *q,
                                     const PortFieldByte *words,size_t count) {
    size_t i;
    if(!s || !q || !q->commands || !words || count!=FA18_STARTUP_WORD_BYTES) return 0;
    for(i=0x23;i<0x64;++i) if(!port_field_byte_valid(q->slots+i)) return 0;
    for(i=0;i<count;++i) if(!port_field_byte_valid(words+i)) return 0;
    *s=(FA18NativeStartupRanges){q,words};
    return 1;
}
int fa18_clear_native_startup_ranges(FA18NativeStartupRanges *s) {
    if(!s || !s->queue || !s->queue->commands) return 0;
    if(!port_fill_field_bytes(s->queue->slots+0x2f,53,0)) return 0;
    return port_fill_field_bytes(s->words,FA18_STARTUP_WORD_BYTES,0);
}
int fa18_enable_native_startup_ranges(FA18NativeStartupRanges *s) {
    if(!s || !s->queue || !s->queue->commands) return 0;
    return port_fill_field_bytes(s->queue->slots+0x23,12,1);
}
