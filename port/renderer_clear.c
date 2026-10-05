#include "renderer_clear.h"
#include <string.h>

typedef struct { uint8_t *bytes; size_t count; } ClearSpan;
static ClearSpan capture(FA18NativeGraphicsPlane *plane) {
    return plane?(ClearSpan){plane->bytes,plane->byte_count}:(ClearSpan){NULL,0};
}
static int clear_long(ClearSpan span,size_t offset) {
    if(!span.bytes || offset>span.count || span.count-offset<4) return 0;
    memset(span.bytes+offset,0,4);
    return 1;
}
int fa18_bind_native_renderer_clear(FA18NativeRendererClear *s,FA18CommandQueue *q) {
    return s && s->graphics && s->fifth_buffer_used &&
           fa18_bind_command_queue_byte(q,0x75,s->fifth_buffer_used);
}
int fa18_clear_native_renderer(FA18NativeRendererClear *s) {
    ClearSpan a[5],b[5];
    size_t i,fifth=0;
    unsigned stream;
    if(!s || !s->graphics || !s->fifth_buffer_used) return 0;
    for(stream=0;stream<5;++stream) a[stream]=capture(s->graphics->source[stream]);
    for(i=0;i<2000;++i) {
        for(stream=0;stream<4;++stream) if(!clear_long(a[stream],4*i)) return 0;
        if(*s->fifth_buffer_used) {
            if(!clear_long(a[4],fifth)) return 0;
            fifth+=4;
        }
    }
    for(stream=0;stream<4;++stream) b[stream]=capture(s->graphics->source[5+stream]);
    b[4]=capture(s->additional_buffer);
    for(i=0;i<2000;++i)
        for(stream=0;stream<5;++stream) if(!clear_long(b[stream],4*i)) return 0;
    return 1;
}
