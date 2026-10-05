#include "rgb4.h"
#include <assert.h>
#include <string.h>

static void word(uint8_t *p,unsigned v) { p[0]=(uint8_t)(v>>8); p[1]=(uint8_t)v; }
static unsigned value(const uint8_t *p) { return (unsigned)p[0]<<8|p[1]; }
int main(void) {
    uint8_t src[64],map[64],ins[30],hardware[20];
    AmigaRgb4Palette p={map,sizeof map};
    AmigaRgb4HardwareList hw={hardware,sizeof hardware};
    AmigaRgb4CopperList list={ins,sizeof ins,5,amiga_rgb4_write_hardware,&hw};
    unsigned i;
    for(i=0;i<32;++i) word(src+2*i,0xf000u+i);
    memset(map,0xa5,sizeof map); memset(ins,0,sizeof ins); memset(hardware,0x5a,sizeof hardware);
    word(ins+2,0x180); word(ins+8,0x19e); word(ins+14,0x1a0); word(ins+20,0x180); word(ins+26,0x180);
    word(ins+18,1); /* WAIT keeps its mask even when its second word resembles COLOR00. */
    assert(amiga_rgb4_load(&p,src,sizeof src,16,&list));
    assert(!memcmp(map,src,32) && map[32]==0xa5);
    assert(value(ins+4)==0 && value(ins+10)==15 && value(ins+16)==0 && value(ins+22)==0);
    assert(value(hardware+2)==0 && value(hardware+6)==15 && value(hardware+10)==0x5a5a && value(hardware+14)==0x5a5a);
    /* Capacity clips the requested count; raw map bits and masked list bits differ. */
    p.byte_count=2;
    assert(amiga_rgb4_load(&p,src,2,32,NULL) && value(map)==0xf000);
    assert(!amiga_rgb4_load(&p,NULL,0,1,NULL));
    p.byte_count=0;
    assert(amiga_rgb4_load(&p,NULL,0,100,NULL));
    /* A failed hardware write occurs after this internal data word changed. */
    p.byte_count=sizeof map; hw.byte_count=4;
    word(ins+10,0xbeef); word(ins+28,0xbeef);
    assert(!amiga_rgb4_load(&p,src,sizeof src,16,&list));
    assert(value(ins+10)==15 && value(ins+28)==0xbeef);
    /* Resolve failures preserve a completed map copy, but leave the list intact. */
    list.instruction_count=6; word(ins+4,0xbeef);
    assert(!amiga_rgb4_load(&p,src,sizeof src,16,&list) && value(ins+4)==0xbeef);
    assert(!amiga_rgb4_write_hardware(NULL,0,0));
    assert(!amiga_rgb4_copy(NULL,src,sizeof src,1));
    /* Ordinary buffers explicitly support overlap without invoking memcpy UB. */
    assert(amiga_rgb4_copy(&p,map+2,sizeof map-2,16));
    assert(value(map)==0xf001);
    return 0;
}
