#include "exec_context.h"
int amiga_exec_context_save(AmigaExecTaskState *s,const AmigaExecTaskBus *b,
                            unsigned base,unsigned mask,unsigned *count) {
    if (!s || !b || !b->write16 || !count || base>=8 || mask>0xFFFF) return 0;
    uint32_t address=s->a[base]; *count=0;
    /* The original 68000 stores the initial base-register value when included
     * in the mask, then updates it after all transfers. Each long writes its
     * low word first. Later CPU variants have different base semantics. */
    for (int i=15;i>=0;--i) if (mask&(1u<<i)) {
        uint32_t value=i<8?s->d[i]:s->a[i-8]; address-=4;
        b->write16(b->context,address+2,(uint16_t)value);
        b->write16(b->context,address,(uint16_t)(value>>16)); ++*count;
    }
    s->a[base]=address; return 1;
}
int amiga_exec_context_restore(AmigaExecTaskState *s,const AmigaExecTaskBus *b,
                               unsigned base,unsigned mask,int postincrement,
                               unsigned *count) {
    if (!s || !b || !b->read32 || !count || base>=8 || mask>0xFFFF) return 0;
    uint32_t address=s->a[base]; *count=0;
    for (unsigned i=0;i<16;++i) if (mask&(1u<<i)) {
        uint32_t value=b->read32(b->context,address); address+=4;
        if (i<8) s->d[i]=value; else s->a[i-8]=value; ++*count;
    }
    if (postincrement) s->a[base]=address;
    return 1;
}
