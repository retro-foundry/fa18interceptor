#include "exec_task_services.h"
#include "abi_13.h"
#include <string.h>

static void flags(AmigaExecTaskState *s,uint32_t value,uint32_t sign) {
    s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!value?AMIGA_CCR_Z:0)|
                    (value&sign?AMIGA_CCR_N:0));
}
static void zero_flag(AmigaExecTaskState *s,int zero) {
    s->ccr=(uint8_t)((s->ccr&~AMIGA_CCR_Z)|(zero?AMIGA_CCR_Z:0));
}
static void byte_compare(AmigaExecTaskState *s,uint8_t source,uint8_t dest) {
    uint8_t result=(uint8_t)(dest-source);
    s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!result?AMIGA_CCR_Z:0)|
        (result&0x80?AMIGA_CCR_N:0)|((source^dest)&(result^dest)&0x80?AMIGA_CCR_V:0)|
        (dest<source?AMIGA_CCR_C:0));
}
static void long_compare(AmigaExecTaskState *s,uint32_t source,uint32_t dest) {
    uint32_t result=dest-source;
    s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!result?AMIGA_CCR_Z:0)|
        (result&0x80000000u?AMIGA_CCR_N:0)|
        ((source^dest)&(result^dest)&0x80000000u?AMIGA_CCR_V:0)|
        (dest<source?AMIGA_CCR_C:0));
}
static void push_long(AmigaExecTaskState *s,const AmigaExecTaskBus *b,uint32_t value) {
    s->a[7]-=4;
    b->write16(b->context,s->a[7]+2,(uint16_t)value);
    b->write16(b->context,s->a[7],(uint16_t)(value>>16));
    flags(s,value,0x80000000u);
}
int amiga_exec_task_step(AmigaExecTaskPhase p,unsigned arg,AmigaExecTaskState *s,
                         const AmigaExecTaskBus *b,AmigaExecTaskEffect *e) {
    uint32_t address,value,old,mask; uint8_t byte;
    if ((unsigned)p>=AMIGA_EXEC_TASK_PHASE_COUNT || !s || !b || !e ||
        !b->read8 || !b->read16 || !b->read32 || !b->write8 || !b->write16 || !b->write32 ||
        ((p==AMIGA_EXEC_PUSH_A || p==AMIGA_EXEC_POP_A) && arg>=8) ||
        (p==AMIGA_EXEC_LIST_PHASE && arg>=AMIGA_LIST_PHASE_COUNT)) return 0;
    memset(e,0,sizeof *e);
    switch (p) {
    case AMIGA_EXEC_FLOW: break; /* Guest calls/transfers belong to the adapter. */
    case AMIGA_EXEC_EQ: e->branch_taken=(s->ccr&AMIGA_CCR_Z)!=0; break;
    case AMIGA_EXEC_NE: e->branch_taken=(s->ccr&AMIGA_CCR_Z)==0; break;
    case AMIGA_EXEC_GE: e->branch_taken=((s->ccr&AMIGA_CCR_N)!=0)==((s->ccr&AMIGA_CCR_V)!=0); break;
    case AMIGA_EXEC_LT: e->branch_taken=((s->ccr&AMIGA_CCR_N)!=0)!=((s->ccr&AMIGA_CCR_V)!=0); break;
    case AMIGA_EXEC_RETURN:
        e->return_pc=b->read32(b->context,s->a[7]); s->a[7]+=4; e->returned=1; break;
    case AMIGA_EXEC_LIST_PHASE: {
        AmigaExecListState ls={s->d[0],s->d[1],s->a[0],s->a[1],s->a[2],s->a[7],0,s->ccr};
        AmigaExecListBus lb={b->context,b->read8,b->read32,b->write32};
        AmigaExecListEffect le;
        if (!amiga_exec_list_step((AmigaExecListPhase)arg,&ls,&lb,&le)) return 0;
        s->d[0]=ls.d0; s->d[1]=ls.d1; s->a[0]=ls.a0; s->a[1]=ls.a1;
        s->a[2]=ls.a2; s->a[7]=ls.sp; s->ccr=ls.ccr; break;
    }
    case AMIGA_EXEC_INTERRUPT_WORD:
        b->write16(b->context,0xDFF09A,(uint16_t)arg); flags(s,(uint16_t)arg,0x8000); break;
    case AMIGA_EXEC_REQUEST_RESCHEDULE_INTERRUPT:
        b->write16(b->context,0xDFF09C,0x8004); flags(s,0x8004,0x8000); break;
    case AMIGA_EXEC_DEPTH_UP: case AMIGA_EXEC_DEPTH_DOWN:
        address=s->a[6]+arg; old=b->read8(b->context,address);
        value=p==AMIGA_EXEC_DEPTH_UP?old+1:old-1;
        s->ccr=(uint8_t)((!(value&255)?AMIGA_CCR_Z:0)|(value&128?AMIGA_CCR_N:0)|
            ((p==AMIGA_EXEC_DEPTH_UP?~(old^1)&(old^value):(old^1)&(old^value))&128?AMIGA_CCR_V:0)|
            (value&256?AMIGA_CCR_C|AMIGA_CCR_X:0));
        b->write8(b->context,address,(uint8_t)value); break;
    case AMIGA_EXEC_CURRENT_TASK: s->a[1]=b->read32(b->context,s->a[6]+AMIGA_EXEC_THIS_TASK); break;
    case AMIGA_EXEC_TASK_FIELD: s->a[0]=s->a[1]+arg; break;
    case AMIGA_EXEC_PUSH_A: push_long(s,b,s->a[arg]); break;
    case AMIGA_EXEC_POP_A: s->a[arg]=b->read32(b->context,s->a[7]); s->a[7]+=4; break;
    case AMIGA_EXEC_PUSH_FIELD: value=b->read32(b->context,s->a[0]); push_long(s,b,value); break;
    case AMIGA_EXEC_POP_D0: s->d[0]=b->read32(b->context,s->a[7]); s->a[7]+=4; flags(s,s->d[0],0x80000000u); break;
    case AMIGA_EXEC_D0_FROM_A1: s->d[0]=s->a[1]; flags(s,s->d[0],0x80000000u); break;
    case AMIGA_EXEC_D1_FROM_A0: s->d[1]=s->a[0]; flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_A1_FROM_D0: s->a[1]=s->d[0]; break;
    case AMIGA_EXEC_A1_FROM_D1: s->a[1]=s->d[1]; break;
    case AMIGA_EXEC_A0_FROM_D1: s->a[0]=s->d[1]; break;
    case AMIGA_EXEC_A2_FROM_A0: s->a[2]=s->a[0]; break;
    case AMIGA_EXEC_A0_FROM_A5: s->a[0]=s->a[5]; break;
    case AMIGA_EXEC_A5_FROM_A0: s->a[5]=s->a[0]; break;
    case AMIGA_EXEC_PORT_LIST: s->a[0]+=AMIGA_PORT_MESSAGES; break;
    case AMIGA_EXEC_PORT_HEAD: s->a[1]=b->read32(b->context,s->a[0]+AMIGA_PORT_MESSAGES); break;
    case AMIGA_EXEC_PORT_OWNER: s->d[1]=b->read32(b->context,s->a[1]+AMIGA_PORT_SIGNAL_TASK); flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_PORT_FLAGS: case AMIGA_EXEC_PORT_BIT_D0: case AMIGA_EXEC_PORT_BIT_D1:
        byte=b->read8(b->context,(p==AMIGA_EXEC_PORT_BIT_D1?s->a[0]:s->a[1])+
                         (p==AMIGA_EXEC_PORT_FLAGS?AMIGA_PORT_FLAGS:AMIGA_PORT_SIGNAL_BIT));
        if (p==AMIGA_EXEC_PORT_BIT_D1) s->d[1]=(s->d[1]&0xFFFFFF00u)|byte;
        else s->d[0]=(s->d[0]&0xFFFFFF00u)|byte;
        flags(s,byte,0x80); break;
    case AMIGA_EXEC_PORT_ACTION_MASK:
        s->d[0]=(s->d[0]&0xFFFF0000u)|((uint16_t)s->d[0]&3); flags(s,(uint16_t)s->d[0],0x8000); break;
    case AMIGA_EXEC_PORT_ACTION_COMPARE: byte_compare(s,(uint8_t)arg,(uint8_t)s->d[0]); break;
    case AMIGA_EXEC_D0_ZERO: s->d[0]=0; flags(s,0,0x80000000u); break;
    case AMIGA_EXEC_D1_ZERO: s->d[1]=0; flags(s,0,0x80000000u); break;
    case AMIGA_EXEC_D0_FROM_D1: s->d[0]=s->d[1]; flags(s,s->d[0],0x80000000u); break;
    case AMIGA_EXEC_MASK_FROM_D0: case AMIGA_EXEC_MASK_FROM_D1:
        mask=1u<<(s->d[p==AMIGA_EXEC_MASK_FROM_D0?0:1]&31);
        old=s->d[p==AMIGA_EXEC_MASK_FROM_D0?1:0]; zero_flag(s,!(old&mask));
        s->d[p==AMIGA_EXEC_MASK_FROM_D0?1:0]=old|mask; e->bit_below16=mask<0x10000; break;
    case AMIGA_EXEC_REPLY_PORT: s->d[0]=b->read32(b->context,s->a[1]+AMIGA_MESSAGE_REPLY_PORT); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_EXEC_MESSAGE_TYPE: b->write8(b->context,s->a[1]+AMIGA_NODE_TYPE,(uint8_t)arg); flags(s,(uint8_t)arg,0x80); break;
    case AMIGA_EXEC_LIST_HEAD_A1: s->a[1]=b->read32(b->context,s->a[2]); break;
    case AMIGA_EXEC_TEST_NODE: flags(s,b->read32(b->context,s->a[1]),0x80000000u); break;
    case AMIGA_EXEC_SIGNALS_MASK_INPUT: s->d[0]&=s->d[1]; flags(s,s->d[0],0x80000000u); break;
    case AMIGA_EXEC_INVERT_D1: s->d[1]=~s->d[1]; flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_MASK_OLD_FIELD: s->d[1]&=b->read32(b->context,s->a[0]); flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_MERGE_SIGNALS: s->d[1]|=s->d[0]; flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_STORE_SIGNALS: b->write32(b->context,s->a[0],s->d[1]); flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_LOAD_RECEIVED_D0: s->d[0]=b->read32(b->context,s->a[1]+AMIGA_TASK_SIGNALS_RECEIVED); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_EXEC_OR_SIGNALS:
        value=b->read32(b->context,s->a[0])|s->d[0]; b->write32(b->context,s->a[0],value); flags(s,value,0x80000000u); break;
    case AMIGA_EXEC_LOAD_EXCEPT_D1: s->d[1]=b->read32(b->context,s->a[1]+AMIGA_TASK_SIGNALS_EXCEPT); flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_INTERSECT_D1: s->d[1]&=s->d[0]; flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_COMPARE_TASK_STATE: byte_compare(s,(uint8_t)arg,b->read8(b->context,s->a[1]+AMIGA_TASK_STATE)); break;
    case AMIGA_EXEC_INTERSECT_WAIT_D0: s->d[0]&=b->read32(b->context,s->a[1]+AMIGA_TASK_SIGNALS_WAIT); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_EXEC_EXEC_LIST: s->a[0]=s->a[6]+arg; break;
    case AMIGA_EXEC_TASK_STATE: b->write8(b->context,s->a[1]+AMIGA_TASK_STATE,(uint8_t)arg); flags(s,(uint8_t)arg,0x80); break;
    case AMIGA_EXEC_COMPARE_READY_HEAD:
        value=b->read32(b->context,s->a[6]+AMIGA_EXEC_TASK_READY);
        long_compare(s,value,s->a[1]); break;
    case AMIGA_EXEC_EXCEPTION_PENDING:
        address=s->a[1]+AMIGA_TASK_FLAGS; byte=b->read8(b->context,address); zero_flag(s,!(byte&32)); b->write8(b->context,address,(uint8_t)(byte|32)); break;
    case AMIGA_EXEC_STORE_WAIT: b->write32(b->context,s->a[1]+AMIGA_TASK_SIGNALS_WAIT,s->d[0]); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_EXEC_LOAD_WAIT: s->d[0]=b->read32(b->context,s->a[1]+AMIGA_TASK_SIGNALS_WAIT); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_EXEC_LOAD_RECEIVED_D1: s->d[1]=b->read32(b->context,s->a[1]+AMIGA_TASK_SIGNALS_RECEIVED); flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_CLEAR_RECEIVED:
        address=s->a[1]+AMIGA_TASK_SIGNALS_RECEIVED; value=b->read32(b->context,address)^s->d[1];
        b->write32(b->context,address,value); flags(s,value,0x80000000u); break;
    case AMIGA_EXEC_RESCHEDULE_PENDING:
        address=s->a[6]+AMIGA_EXEC_RESCHEDULE_FLAGS; byte=b->read8(b->context,address); zero_flag(s,!(byte&128)); b->write8(b->context,address,(uint8_t)(byte|128)); break;
    case AMIGA_EXEC_PENDING_TO_D0:
        e->scc_true=(s->ccr&AMIGA_CCR_Z)==0; s->d[0]=(s->d[0]&0xFFFFFF00u)|(e->scc_true?255:0); break;
    case AMIGA_EXEC_TEST_DEPTH: flags(s,b->read8(b->context,s->a[6]+arg),0x80); break;
    case AMIGA_EXEC_TEST_D0_BYTE: flags(s,(uint8_t)s->d[0],0x80); break;
    case AMIGA_EXEC_TEST_RESCHEDULE: zero_flag(s,!(b->read8(b->context,s->a[6]+AMIGA_EXEC_RESCHEDULE_FLAGS)&128)); break;
    case AMIGA_EXEC_LOAD_ALLOCATED_SIGNALS: s->d[1]=b->read32(b->context,s->a[1]+AMIGA_TASK_SIGNALS_ALLOCATED); flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_LOAD_ALLOCATED_TRAPS: value=b->read16(b->context,s->a[1]+AMIGA_TASK_TRAPS_ALLOCATED); s->d[1]=(s->d[1]&0xFFFF0000u)|value; flags(s,value,0x8000); break;
    case AMIGA_EXEC_COMPARE_ANY_BIT: byte_compare(s,255,(uint8_t)s->d[0]); break;
    case AMIGA_EXEC_ALLOCATE_BIT: case AMIGA_EXEC_FREE_BIT:
        mask=1u<<(s->d[0]&31); zero_flag(s,!(s->d[1]&mask));
        if (p==AMIGA_EXEC_ALLOCATE_BIT) s->d[1]|=mask; else s->d[1]&=~mask;
        e->bit_below16=mask<0x10000; break;
    case AMIGA_EXEC_FIRST_FREE_BIT: s->d[0]=arg; flags(s,arg,0x80000000u); break;
    case AMIGA_EXEC_NEXT_FREE_BIT:
        s->d[0]=(s->d[0]&0xFFFF0000u)|((s->d[0]-1)&0xFFFF); e->dbra_continues=(uint16_t)s->d[0]!=0xFFFF; break;
    case AMIGA_EXEC_NO_FREE_BIT: s->d[0]=UINT32_MAX; flags(s,s->d[0],0x80000000u); break;
    case AMIGA_EXEC_STORE_ALLOCATED_SIGNALS: b->write32(b->context,s->a[1]+AMIGA_TASK_SIGNALS_ALLOCATED,s->d[1]); flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_STORE_ALLOCATED_TRAPS: b->write16(b->context,s->a[1]+AMIGA_TASK_TRAPS_ALLOCATED,(uint16_t)s->d[1]); flags(s,(uint16_t)s->d[1],0x8000); break;
    case AMIGA_EXEC_CLEAR_MASK: s->d[1]=UINT32_MAX; flags(s,s->d[1],0x80000000u); break;
    case AMIGA_EXEC_CLEAR_TASK_MASK:
        address=s->a[1]+arg; value=b->read32(b->context,address)&s->d[1]; b->write32(b->context,address,value); flags(s,value,0x80000000u); break;
    case AMIGA_EXEC_PUSH_RETURN_PC:
        s->a[7]-=4; b->write32(b->context,s->a[7],arg); break;
    case AMIGA_EXEC_PUSH_SR:
        s->a[7]-=2; b->write16(b->context,s->a[7],(uint16_t)arg); break;
    case AMIGA_EXEC_COMPARE_EXCEPTION_CALL:
        value=b->read32(b->context,s->a[7]+2); long_compare(s,arg,value); break;
    case AMIGA_EXEC_SET_EXCEPTION_RETURN:
        b->write32(b->context,s->a[7]+2,arg); flags(s,arg,0x80000000u); break;
    case AMIGA_EXEC_TEST_SAVED_SUPERVISOR:
        zero_flag(s,!(b->read8(b->context,s->a[7])&32)); break;
    default: return 0;
    }
    return 1;
}
