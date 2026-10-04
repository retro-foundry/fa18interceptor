#include "exec_scheduler.h"
#include "abi_13.h"
static void flags(AmigaExecTaskState *s,uint32_t v,uint32_t sign) {
    s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!v?AMIGA_CCR_Z:0)|(v&sign?AMIGA_CCR_N:0));
}
static void test_bit(AmigaExecTaskState *s,uint32_t v,unsigned bit) {
    s->ccr=(uint8_t)((s->ccr&~AMIGA_CCR_Z)|(!(v&(1u<<bit))?AMIGA_CCR_Z:0));
}
static void compare(AmigaExecTaskState *s,uint32_t dest,uint32_t src,uint32_t sign) {
    uint32_t mask=sign|(sign-1),v=(dest-src)&mask;
    s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!v?AMIGA_CCR_Z:0)|(v&sign?AMIGA_CCR_N:0)|
        ((dest^src)&(dest^v)&sign?AMIGA_CCR_V:0)|(dest<src?AMIGA_CCR_C:0));
}
static void move_long(AmigaExecTaskState *s,const AmigaExecTaskBus *b,uint32_t a,uint32_t v) {
    b->write32(b->context,a,v); flags(s,v,0x80000000u);
}
static void move_word(AmigaExecTaskState *s,const AmigaExecTaskBus *b,uint32_t a,uint16_t v) {
    b->write16(b->context,a,v); flags(s,v,0x8000);
}
static void push_long(AmigaExecTaskState *s,const AmigaExecTaskBus *b,unsigned base,uint32_t v) {
    s->a[base]-=4;
    b->write16(b->context,s->a[base]+2,(uint16_t)v);
    b->write16(b->context,s->a[base],(uint16_t)(v>>16)); flags(s,v,0x80000000u);
}
int amiga_exec_scheduler_step(AmigaExecSchedulerPhase p,unsigned arg,
                              AmigaExecTaskState *s,const AmigaExecTaskBus *b) {
    uint32_t a,v,old; uint8_t byte;
    if ((unsigned)p>=AMIGA_SCHED_PHASE_COUNT || !s || !b || !b->read8 || !b->read16 ||
        !b->read32 || !b->write8 || !b->write16 || !b->write32 ||
        ((p==AMIGA_SCHED_TEST_RESCHEDULE_BIT || p==AMIGA_SCHED_CLEAR_RESCHEDULE_BIT ||
          p==AMIGA_SCHED_SET_RESCHEDULE_BIT || p==AMIGA_SCHED_TEST_CPU_FLAG) && arg>=8) ||
        (p==AMIGA_SCHED_TEST_CALLBACK_BIT && arg>=32)) return 0;
    switch (p) {
    case AMIGA_SCHED_SAVED_SUPERVISOR: test_bit(s,b->read8(b->context,s->a[7]+24),5); break;
    case AMIGA_SCHED_EXEC_BASE: s->a[6]=b->read32(b->context,4); break;
    case AMIGA_SCHED_TEST_TASK_EXCEPTION: test_bit(s,b->read8(b->context,s->a[1]+AMIGA_TASK_FLAGS),5); break;
    case AMIGA_SCHED_TEST_TASK_SWITCH: test_bit(s,b->read8(b->context,s->a[3]+AMIGA_TASK_FLAGS),6); break;
    case AMIGA_SCHED_TEST_RESCHEDULE_BIT:
        test_bit(s,b->read8(b->context,s->a[6]+AMIGA_EXEC_RESCHEDULE_FLAGS),arg); break;
    case AMIGA_SCHED_CLEAR_RESCHEDULE_BIT: case AMIGA_SCHED_SET_RESCHEDULE_BIT:
        a=s->a[6]+AMIGA_EXEC_RESCHEDULE_FLAGS; byte=b->read8(b->context,a); test_bit(s,byte,arg);
        b->write8(b->context,a,(uint8_t)(p==AMIGA_SCHED_SET_RESCHEDULE_BIT?byte|(1u<<arg):byte&~(1u<<arg))); break;
    case AMIGA_SCHED_READY_EMPTY:
        compare(s,s->a[0],b->read32(b->context,s->a[0]+AMIGA_LIST_TAIL_PRED),0x80000000u); break;
    case AMIGA_SCHED_READY_HEAD_A0: s->a[0]=b->read32(b->context,s->a[0]); break;
    case AMIGA_SCHED_READY_PRIORITY:
        byte=b->read8(b->context,s->a[0]+AMIGA_NODE_PRIORITY);
        s->d[1]=(s->d[1]&0xFFFFFF00u)|byte; flags(s,byte,0x80); break;
    case AMIGA_SCHED_COMPARE_PRIORITY:
        compare(s,(uint8_t)s->d[1],b->read8(b->context,s->a[1]+AMIGA_NODE_PRIORITY),0x80); break;
    case AMIGA_SCHED_PUSH_SAVED_A6: push_long(s,b,7,b->read32(b->context,s->a[7])); break;
    case AMIGA_SCHED_INSTALL_SWITCH_RETURN:
        move_long(s,b,s->a[7]+4,b->read32(b->context,s->a[6]-52)); break;
    case AMIGA_SCHED_LOAD_NEST_COUNTS:
        v=b->read16(b->context,s->a[6]+AMIGA_EXEC_ID_NEST_CNT);
        s->d[0]=(s->d[0]&0xFFFF0000u)|v; flags(s,v,0x8000); break;
    case AMIGA_SCHED_RESET_NEST_COUNTS: move_word(s,b,s->a[6]+AMIGA_EXEC_ID_NEST_CNT,0xFFFF); break;
    case AMIGA_SCHED_SAVE_CALLER_A5:
        v=b->read32(b->context,s->a[7]); s->a[7]+=4; move_long(s,b,s->a[5]+52,v); break;
    case AMIGA_SCHED_SAVE_STATUS:
        v=b->read16(b->context,s->a[7]); s->a[7]+=2; s->a[5]-=2;
        move_word(s,b,s->a[5],(uint16_t)v); break;
    case AMIGA_SCHED_SAVE_PC:
        v=b->read32(b->context,s->a[7]); s->a[7]+=4; push_long(s,b,5,v); break;
    case AMIGA_SCHED_CURRENT_TASK_A3: s->a[3]=b->read32(b->context,s->a[6]+AMIGA_EXEC_THIS_TASK); break;
    case AMIGA_SCHED_SAVE_TASK_NEST_COUNTS: move_word(s,b,s->a[3]+AMIGA_TASK_NEST_COUNTS,(uint16_t)s->d[0]); break;
    case AMIGA_SCHED_SAVE_TASK_SP: move_long(s,b,s->a[3]+AMIGA_TASK_SAVED_SP,s->a[5]); break;
    case AMIGA_SCHED_LOAD_SWITCH_CALLBACK: s->a[5]=b->read32(b->context,s->a[3]+AMIGA_TASK_SWITCH); break;
    case AMIGA_SCHED_READY_HEAD_A3: s->a[3]=b->read32(b->context,s->a[0]); break;
    case AMIGA_SCHED_READY_SUCCESSOR: s->d[0]=b->read32(b->context,s->a[3]); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_SCHED_INCREMENT_COUNTER:
        a=s->a[6]+arg; old=b->read32(b->context,a); v=old+1;
        s->ccr=(uint8_t)((!v?AMIGA_CCR_Z:0)|(v&0x80000000u?AMIGA_CCR_N:0)|
            (~(old^1)&(old^v)&0x80000000u?AMIGA_CCR_V:0)|(!v?AMIGA_CCR_X|AMIGA_CCR_C:0));
        b->write32(b->context,a,v); break;
    case AMIGA_SCHED_REMOVE_READY_HEAD: move_long(s,b,s->a[0],s->d[0]); break;
    case AMIGA_SCHED_READY_SUCCESSOR_A1: s->a[1]=s->d[0]; break;
    case AMIGA_SCHED_LINK_READY_SUCCESSOR: move_long(s,b,s->a[1]+AMIGA_NODE_PRED,s->a[0]); break;
    case AMIGA_SCHED_INSTALL_CURRENT_TASK: move_long(s,b,s->a[6]+AMIGA_EXEC_THIS_TASK,s->a[3]); break;
    case AMIGA_SCHED_RESET_QUANTUM:
        move_word(s,b,s->a[6]+AMIGA_EXEC_QUANTUM_REMAINING,b->read16(b->context,s->a[6]+AMIGA_EXEC_QUANTUM)); break;
    case AMIGA_SCHED_SET_TASK_STATE:
        b->write8(b->context,s->a[3]+AMIGA_TASK_STATE,(uint8_t)arg); flags(s,(uint8_t)arg,0x80); break;
    case AMIGA_SCHED_RESTORE_NEST_COUNTS:
        move_word(s,b,s->a[6]+AMIGA_EXEC_ID_NEST_CNT,b->read16(b->context,s->a[3]+AMIGA_TASK_NEST_COUNTS)); break;
    case AMIGA_SCHED_LOAD_TASK_FLAGS:
        byte=b->read8(b->context,s->a[3]+AMIGA_TASK_FLAGS);
        s->d[0]=(s->d[0]&0xFFFFFF00u)|byte; flags(s,byte,0x80); break;
    case AMIGA_SCHED_CALLBACK_FLAGS: s->d[0]&=0xFFFFFFA0u; flags(s,(uint8_t)s->d[0],0x80); break;
    case AMIGA_SCHED_LOAD_TASK_SP_A5: s->a[5]=b->read32(b->context,s->a[3]+AMIGA_TASK_SAVED_SP); break;
    case AMIGA_SCHED_RESUMED_USP_A2: s->a[2]=s->a[5]+66; break;
    case AMIGA_SCHED_PUSH_TASK_PC:
        v=b->read32(b->context,s->a[5]); s->a[5]+=4; push_long(s,b,7,v); break;
    case AMIGA_SCHED_PUSH_TASK_STATUS:
        v=b->read16(b->context,s->a[5]); s->a[5]+=2; s->a[7]-=2;
        move_word(s,b,s->a[7],(uint16_t)v); break;
    case AMIGA_SCHED_TEST_CALLBACK_BIT: test_bit(s,s->d[0],arg); break;
    case AMIGA_SCHED_SAVE_CALLBACK_FLAGS:
        s->d[2]=(s->d[2]&0xFFFFFF00u)|(uint8_t)s->d[0]; flags(s,(uint8_t)s->d[2],0x80); break;
    case AMIGA_SCHED_RESTORE_CALLBACK_FLAGS:
        s->d[0]=(s->d[0]&0xFFFFFF00u)|(uint8_t)s->d[2]; flags(s,(uint8_t)s->d[0],0x80); break;
    case AMIGA_SCHED_LOAD_LAUNCH_CALLBACK: s->a[5]=b->read32(b->context,s->a[3]+AMIGA_TASK_LAUNCH); break;
    case AMIGA_SCHED_CLEAR_TASK_EXCEPTION:
        a=s->a[3]+AMIGA_TASK_FLAGS; byte=b->read8(b->context,a); test_bit(s,byte,5);
        b->write8(b->context,a,(uint8_t)(byte&~32)); break;
    case AMIGA_SCHED_LOAD_RECEIVED:
        s->d[0]=b->read32(b->context,s->a[3]+AMIGA_TASK_SIGNALS_RECEIVED); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_SCHED_EXCEPTION_SIGNALS:
        s->d[0]&=b->read32(b->context,s->a[3]+AMIGA_TASK_SIGNALS_EXCEPT); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_SCHED_CLEAR_SIGNALS:
        a=s->a[3]+arg; move_long(s,b,a,b->read32(b->context,a)^s->d[0]); break;
    case AMIGA_SCHED_LOAD_TASK_SP_A1: s->a[1]=b->read32(b->context,s->a[3]+AMIGA_TASK_SAVED_SP); break;
    case AMIGA_SCHED_PUSH_TASK_HEADER: push_long(s,b,1,b->read32(b->context,s->a[3]+AMIGA_TASK_FLAGS)); break;
    case AMIGA_SCHED_PUSH_EXCEPTION_RETURN: push_long(s,b,1,arg); break;
    case AMIGA_SCHED_TEST_CPU_FLAG: test_bit(s,b->read8(b->context,s->a[6]+AMIGA_EXEC_ATTN_FLAGS_LOW),arg); break;
    case AMIGA_SCHED_PUSH_FRAME_FORMAT: s->a[7]-=2; move_word(s,b,s->a[7],0x20); break;
    case AMIGA_SCHED_PUSH_EXCEPTION_CODE: push_long(s,b,7,b->read32(b->context,s->a[3]+AMIGA_TASK_EXCEPT_CODE)); break;
    case AMIGA_SCHED_PUSH_USER_STATUS:
        s->a[7]-=2; move_word(s,b,s->a[7],0); break;
    case AMIGA_SCHED_EXCEPTION_DATA: s->a[1]=b->read32(b->context,s->a[3]+AMIGA_TASK_EXCEPT_DATA); break;
    case AMIGA_SCHED_ADD_RETURNED_SIGNALS:
        a=s->a[3]+AMIGA_TASK_SIGNALS_EXCEPT; move_long(s,b,a,b->read32(b->context,a)|s->d[0]); break;
    case AMIGA_SCHED_RESTORE_TASK_HEADER:
        v=b->read32(b->context,s->a[1]); s->a[1]+=4; move_long(s,b,s->a[3]+AMIGA_TASK_FLAGS,v); break;
    case AMIGA_SCHED_SAVE_EXCEPTION_SP: move_long(s,b,s->a[3]+AMIGA_TASK_SAVED_SP,s->a[1]); break;
    default: return 0;
    }
    return 1;
}
