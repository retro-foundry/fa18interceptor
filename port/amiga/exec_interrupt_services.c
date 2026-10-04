#include "exec_interrupt_services.h"
#include "abi_13.h"
#include <string.h>
static void flags(AmigaExecTaskState *s,uint32_t v,uint32_t sign) {
    s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!v?AMIGA_CCR_Z:0)|(v&sign?AMIGA_CCR_N:0));
}
static void zero(AmigaExecTaskState *s,int is_zero) {
    s->ccr=(uint8_t)((s->ccr&~AMIGA_CCR_Z)|(is_zero?AMIGA_CCR_Z:0));
}
static void move_long(AmigaExecTaskState *s,const AmigaExecTaskBus *b,uint32_t a,uint32_t v) {
    b->write32(b->context,a,v); flags(s,v,0x80000000u);
}
static void push_long(AmigaExecTaskState *s,const AmigaExecTaskBus *b,uint32_t v) {
    s->a[7]-=4; b->write16(b->context,s->a[7]+2,(uint16_t)v);
    b->write16(b->context,s->a[7],(uint16_t)(v>>16)); flags(s,v,0x80000000u);
}
int amiga_exec_interrupt_step(AmigaExecInterruptPhase p,unsigned arg,AmigaExecTaskState *s,
                              const AmigaExecTaskBus *b,AmigaExecInterruptEffect *e) {
    uint32_t a,v,old,mask; uint8_t byte;
    if ((unsigned)p>=AMIGA_INT_PHASE_COUNT || !s || !b || !e || !b->read8 || !b->read16 ||
        !b->read32 || !b->write8 || !b->write16 || !b->write32 ||
        (p==AMIGA_INT_NODE_CALLBACK && arg>=8) || (p==AMIGA_INT_TEST_PENDING && arg>=32)) return 0;
    memset(e,0,sizeof *e);
    switch (p) {
    case AMIGA_INT_VECTOR_INDEX:
        s->d[0]=(uint16_t)s->d[0]*AMIGA_INT_VECTOR_BYTES; flags(s,s->d[0],0x80000000u); break;
    case AMIGA_INT_VECTOR_ADDRESS:
        s->a[0]=s->a[6]+AMIGA_EXEC_INT_VECTORS+(uint32_t)(int32_t)(int16_t)s->d[0]; break;
    case AMIGA_INT_OLD_NODE: s->d[0]=b->read32(b->context,s->a[0]+AMIGA_INT_VECTOR_NODE); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_INT_INSTALL_NODE: move_long(s,b,s->a[0]+AMIGA_INT_VECTOR_NODE,s->a[1]); break;
    case AMIGA_INT_COPY_DATA: move_long(s,b,s->a[0]+AMIGA_INT_VECTOR_DATA,b->read32(b->context,s->a[1]+AMIGA_INTERRUPT_DATA)); break;
    case AMIGA_INT_COPY_CODE: move_long(s,b,s->a[0]+AMIGA_INT_VECTOR_CODE,b->read32(b->context,s->a[1]+AMIGA_INTERRUPT_CODE)); break;
    case AMIGA_INT_NO_HANDLER: s->d[1]=UINT32_MAX; flags(s,s->d[1],0x80000000u); break;
    case AMIGA_INT_CLEAR_VECTOR: move_long(s,b,s->a[0]+arg,s->d[1]); break;
    case AMIGA_INT_SAVE_D2: push_long(s,b,s->d[2]); break;
    case AMIGA_INT_RESTORE_D2:
        s->d[2]=b->read32(b->context,s->a[7]); s->a[7]+=4; flags(s,s->d[2],0x80000000u); break;
    case AMIGA_INT_SAVE_NUMBER: s->d[2]=s->d[0]; flags(s,s->d[2],0x80000000u); break;
    case AMIGA_INT_NUMBER_D1: s->d[1]=s->d[0]; flags(s,s->d[1],0x80000000u); break;
    case AMIGA_INT_SERVER_LIST: s->a[0]=b->read32(b->context,s->a[0]+AMIGA_INT_VECTOR_DATA); break;
    case AMIGA_INT_SET_ENABLE_BIT:
        s->d[0]=(s->d[0]&0xFFFF0000u)|0x8000; flags(s,0x8000,0x8000); break;
    case AMIGA_INT_ENABLE_WORD: case AMIGA_INT_CLEAR_ENABLE_BIT: case AMIGA_INT_DISABLE_WORD:
        if (p==AMIGA_INT_CLEAR_ENABLE_BIT) { s->d[1]=0; flags(s,0,0x80000000u); break; }
        mask=1u<<(s->d[2]&31); old=s->d[p==AMIGA_INT_ENABLE_WORD?0:1]; zero(s,!(old&mask));
        s->d[p==AMIGA_INT_ENABLE_WORD?0:1]=old|mask; e->bit_below16=mask<0x10000; break;
    case AMIGA_INT_LIST_FROM_D1: s->a[0]=s->d[1]; break;
    case AMIGA_INT_LIST_EMPTY:
        v=b->read32(b->context,s->a[0]+AMIGA_LIST_TAIL_PRED); old=s->a[0]; a=old-v;
        s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!a?AMIGA_CCR_Z:0)|(a&0x80000000u?AMIGA_CCR_N:0)|
            ((old^v)&(old^a)&0x80000000u?AMIGA_CCR_V:0)|(old<v?AMIGA_CCR_C:0)); break;
    case AMIGA_INT_PUSH_CLEAR_MASK:
        v=b->read16(b->context,s->a[1]+18); s->a[7]-=2;
        b->write16(b->context,s->a[7],(uint16_t)v); flags(s,v,0x8000); break;
    case AMIGA_INT_SAVE_A2: push_long(s,b,s->a[2]); break;
    case AMIGA_INT_RESTORE_A2: s->a[2]=b->read32(b->context,s->a[7]); s->a[7]+=4; break;
    case AMIGA_INT_FIRST_SERVER: s->a[2]=b->read32(b->context,s->a[1]); break;
    case AMIGA_INT_SERVER_SUCCESSOR: s->d[0]=b->read32(b->context,s->a[2]); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_INT_NODE_CALLBACK:
        return amiga_exec_context_restore_at(s,b,s->a[arg]+AMIGA_INTERRUPT_DATA,0x2200,&e->transferred_longs);
    case AMIGA_INT_NEXT_SERVER: s->a[2]=b->read32(b->context,s->a[2]); break;
    case AMIGA_INT_ACK_SAVED_MASK:
        v=b->read16(b->context,s->a[7]); s->a[7]+=2; b->write16(b->context,0xDFF09C,(uint16_t)v); flags(s,v,0x8000); break;
    case AMIGA_INT_COMPARE_QUEUED:
        byte=b->read8(b->context,s->a[1]+AMIGA_NODE_TYPE); v=(uint8_t)(byte-11);
        s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!v?AMIGA_CCR_Z:0)|(v&128?AMIGA_CCR_N:0)|
            ((byte^11)&(byte^v)&128?AMIGA_CCR_V:0)|(byte<11?AMIGA_CCR_C:0)); break;
    case AMIGA_INT_QUEUE_NODE: case AMIGA_INT_CALLBACK_NODE_TYPE:
        b->write8(b->context,s->a[1]+AMIGA_NODE_TYPE,(uint8_t)(p==AMIGA_INT_QUEUE_NODE?11:2));
        flags(s,p==AMIGA_INT_QUEUE_NODE?11:2,0x80); break;
    case AMIGA_INT_PRIORITY_BYTE:
        byte=b->read8(b->context,s->a[1]+AMIGA_NODE_PRIORITY); s->d[0]=(s->d[0]&0xFFFFFF00u)|byte; flags(s,byte,0x80); break;
    case AMIGA_INT_PRIORITY_GROUP:
        s->d[0]=(s->d[0]&0xFFFF0000u)|((uint16_t)s->d[0]&0xF0); flags(s,(uint16_t)s->d[0],0x8000); break;
    case AMIGA_INT_SIGN_PRIORITY:
        v=(uint16_t)(int16_t)(int8_t)s->d[0]; s->d[0]=(s->d[0]&0xFFFF0000u)|v; flags(s,v,0x8000); break;
    case AMIGA_INT_SOFT_QUEUE: s->a[0]+=(uint32_t)(int32_t)(int16_t)s->d[0]; break;
    case AMIGA_INT_SOFT_PENDING: case AMIGA_INT_CLEAR_SOFT_PENDING:
        a=s->a[6]+AMIGA_EXEC_RESCHEDULE_FLAGS; byte=b->read8(b->context,a); zero(s,!(byte&32));
        b->write8(b->context,a,(uint8_t)(p==AMIGA_INT_SOFT_PENDING?byte|32:byte&~32)); break;
    case AMIGA_INT_SOFT_HEAD: s->a[1]=b->read32(b->context,s->a[0]); break;
    case AMIGA_INT_SOFT_SUCCESSOR: s->d[0]=b->read32(b->context,s->a[1]); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_INT_UNLINK_SOFT: move_long(s,b,s->a[0],s->d[0]); break;
    case AMIGA_INT_EXCHANGE_NODE: v=s->d[0]; s->d[0]=s->a[1]; s->a[1]=v; break;
    case AMIGA_INT_LINK_SOFT_SUCCESSOR: move_long(s,b,s->a[1]+AMIGA_NODE_PRED,s->a[0]); break;
    case AMIGA_INT_NODE_FROM_D0: s->a[1]=s->d[0]; break;
    case AMIGA_INT_SOFT_SCAN: s->d[0]=4; flags(s,4,0x80000000u); break;
    case AMIGA_INT_SOFT_NEXT_QUEUE: s->a[0]-=AMIGA_SOFT_INT_LIST_BYTES; break;
    case AMIGA_INT_NEXT_PRIORITY:
        s->d[0]=(s->d[0]&0xFFFF0000u)|((s->d[0]-1)&0xFFFF); e->dbra_continues=(uint16_t)s->d[0]!=0xFFFF; break;
    case AMIGA_INT_CUSTOM_BASE: s->a[0]=0xDFF000; break;
    case AMIGA_INT_ENABLED_WORD: s->d[1]=(s->d[1]&0xFFFF0000u)|b->read16(b->context,s->a[0]+0x1C); flags(s,(uint16_t)s->d[1],0x8000); break;
    case AMIGA_INT_PENDING_WORD: case AMIGA_INT_MASK_ENABLED_WORD:
        s->d[1]=(s->d[1]&0xFFFF0000u)|((uint16_t)s->d[1]&b->read16(b->context,s->a[0]+(p==AMIGA_INT_PENDING_WORD?0x1E:0x1C)));
        flags(s,(uint16_t)s->d[1],0x8000); break;
    case AMIGA_INT_TEST_PENDING: zero(s,!(s->d[1]&(1u<<arg))); break;
    case AMIGA_INT_VECTOR_CALLBACK:
        return amiga_exec_context_restore_at(s,b,s->a[6]+arg,0x2200,&e->transferred_longs);
    case AMIGA_INT_AUDIO_MASK: s->d[1]=(s->d[1]&0xFFFF0000u)|0x780; flags(s,0x780,0x8000); break;
    case AMIGA_INT_REQUEST_WORD:
        b->write16(b->context,0xDFF09C,(uint16_t)arg); flags(s,(uint16_t)arg,0x8000); break;
    default: return 0;
    }
    return 1;
}
