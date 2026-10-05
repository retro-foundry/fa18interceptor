#ifndef PORT_FIELD_WINDOW_H
#define PORT_FIELD_WINDOW_H
#include "field_bytes.h"
#include <stddef.h>
#include <stdint.h>

/* A signed-offset view centred inside immutable source data. Reads that leave
 * the source object require explicit adjacent field owners. This keeps asset
 * adapters independent of guest addresses and host byte order. */
typedef struct {
    const uint8_t *bytes;
    size_t byte_count,origin;
    const PortFieldByte *before,*after;
    size_t before_count,after_count;
} PortFieldWindow;

static inline int port_field_window_byte(const PortFieldWindow *w,int32_t offset,uint8_t *value) {
    size_t index,magnitude,distance;
    if(!w || !w->bytes || !value || w->origin>w->byte_count) return 0;
    if(offset<0) {
        magnitude=(size_t)(-(int64_t)offset);
        if(magnitude>w->origin) {
            distance=magnitude-w->origin;
            return w->before && distance<=w->before_count &&
                port_read_field_byte(w->before+w->before_count-distance,value);
        }
        index=w->origin-magnitude;
    } else {
        if((size_t)offset>SIZE_MAX-w->origin) return 0;
        index=w->origin+(size_t)offset;
    }
    if(index<w->byte_count) { *value=w->bytes[index]; return 1; }
    index-=w->byte_count;
    return w->after && index<w->after_count && port_read_field_byte(w->after+index,value);
}
static inline int port_field_window_u16(const PortFieldWindow *w,int32_t offset,uint16_t *value) {
    uint8_t high,low;
    if(!value || !port_field_window_byte(w,offset,&high) || offset==INT32_MAX ||
       !port_field_window_byte(w,offset+1,&low)) return 0;
    *value=(uint16_t)(((unsigned)high<<8)|low); return 1;
}
static inline int port_field_window_s16(const PortFieldWindow *w,int32_t offset,int16_t *value) {
    uint16_t bits;
    if(!value || !port_field_window_u16(w,offset,&bits)) return 0;
    *value=bits<0x8000u?(int16_t)bits:(int16_t)((int32_t)bits-0x10000); return 1;
}
static inline int port_field_window_u32(const PortFieldWindow *w,int32_t offset,uint32_t *value) {
    uint8_t byte; uint32_t result=0; unsigned i;
    if(!value || offset>INT32_MAX-3) return 0;
    for(i=0;i<4;++i) {
        if(!port_field_window_byte(w,offset+(int32_t)i,&byte)) return 0;
        result=(result<<8)|byte;
    }
    *value=result; return 1;
}
#endif
