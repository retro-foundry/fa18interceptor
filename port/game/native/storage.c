#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
static NativeStorage *active;
void native_storage_bind(NativeStorage *storage) { active=storage; }
uint8_t *native_storage_range(uint32_t address,size_t bytes) {
    uint32_t offset=address;
    uint8_t *base=NULL;
    if(active) {
        if(address<sizeof active->buffers) base=active->buffers;
        else if(address>=0xc00000u && address<0xc80000u) { base=active->source; offset-=0xc00000u; }
    }
    if(!base || bytes>0x80000u-offset) {
        fprintf(stderr,"native data range invalid: %08X + %zu\n",address,bytes); abort();
    }
    return base+offset;
}
uint8_t native_data_read8(uint32_t a) { return *native_storage_range(a,1); }
uint16_t native_data_read16(uint32_t a) { const uint8_t *p=native_storage_range(a,2); return (uint16_t)(p[0]<<8|p[1]); }
uint32_t native_data_read32(uint32_t a) { return (uint32_t)native_data_read16(a)<<16|native_data_read16(a+2); }
void native_data_write8(uint32_t a,uint8_t v) { *native_storage_range(a,1)=v; }
void native_data_write16(uint32_t a,uint16_t v) { uint8_t *p=native_storage_range(a,2); p[0]=(uint8_t)(v>>8); p[1]=(uint8_t)v; }
void native_data_write32(uint32_t a,uint32_t v) { native_data_write16(a,(uint16_t)(v>>16)); native_data_write16(a+2,(uint16_t)v); }
