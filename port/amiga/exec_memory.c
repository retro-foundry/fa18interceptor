#include "exec_memory.h"
#include "exec_context.h"
#include "abi_13.h"
#include <string.h>
static void flags(AmigaExecTaskState *s,uint32_t v,uint32_t sign) {
    s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!v?AMIGA_CCR_Z:0)|(v&sign?AMIGA_CCR_N:0));
}
static uint32_t arithmetic(AmigaExecTaskState *s,uint32_t old,uint32_t src,int subtract,int compare) {
    uint32_t v=subtract?old-src:old+src;
    uint32_t overflow=(subtract?(old^src):~(old^src))&(old^v);
    int carry=subtract?old<src:v<old;
    s->ccr=(uint8_t)((compare?s->ccr&AMIGA_CCR_X:carry?AMIGA_CCR_X:0)|
        (!v?AMIGA_CCR_Z:0)|(v&0x80000000u?AMIGA_CCR_N:0)|
        (overflow&0x80000000u?AMIGA_CCR_V:0)|(carry?AMIGA_CCR_C:0));
    return v;
}
static void write_move(AmigaExecTaskState *s,const AmigaExecTaskBus *b,uint32_t a,uint32_t v) {
    b->write32(b->context,a,v); flags(s,v,0x80000000u);
}
int amiga_exec_memory_step(AmigaExecMemoryPhase p,unsigned arg,AmigaExecTaskState *s,
                           const AmigaExecTaskBus *b,AmigaExecMemoryEffect *e) {
    uint32_t v,a,old; unsigned bit;
    if ((unsigned)p>=AMIGA_MEM_PHASE_COUNT || !s || !b || !e || !b->read8 || !b->read16 ||
        !b->read32 || !b->write8 || !b->write16 || !b->write32 ||
        ((p==AMIGA_MEM_SAVE_REGISTERS || p==AMIGA_MEM_RESTORE_REGISTERS) && arg>0xFFFF)) return 0;
    memset(e,0,sizeof *e);
    switch (p) {
    case AMIGA_MEM_FLOW: break;
    case AMIGA_MEM_EQ: e->branch_taken=(s->ccr&AMIGA_CCR_Z)!=0; break;
    case AMIGA_MEM_NE: e->branch_taken=(s->ccr&AMIGA_CCR_Z)==0; break;
    case AMIGA_MEM_HI: e->branch_taken=(s->ccr&(AMIGA_CCR_C|AMIGA_CCR_Z))==0; break;
    case AMIGA_MEM_LS: e->branch_taken=(s->ccr&(AMIGA_CCR_C|AMIGA_CCR_Z))!=0; break;
    case AMIGA_MEM_CS: e->branch_taken=(s->ccr&AMIGA_CCR_C)!=0; break;
    case AMIGA_MEM_CC: e->branch_taken=(s->ccr&AMIGA_CCR_C)==0; break;
    case AMIGA_MEM_GE: e->branch_taken=((s->ccr&AMIGA_CCR_N)!=0)==((s->ccr&AMIGA_CCR_V)!=0); break;
    case AMIGA_MEM_GT: e->branch_taken=!(s->ccr&AMIGA_CCR_Z) && ((s->ccr&AMIGA_CCR_N)!=0)==((s->ccr&AMIGA_CCR_V)!=0); break;
    case AMIGA_MEM_SAVE_REGISTERS:
        return amiga_exec_context_save(s,b,7,arg,&e->transferred_longs);
    case AMIGA_MEM_RESTORE_REGISTERS:
        return amiga_exec_context_restore(s,b,7,arg,1,&e->transferred_longs);
    case AMIGA_MEM_RETURN: e->return_pc=b->read32(b->context,s->a[7]); s->a[7]+=4; e->returned=1; break;
    case AMIGA_MEM_TEST_SIZE: flags(s,s->d[0],0x80000000u); break;
    case AMIGA_MEM_ROUND_ADD: s->d[0]=arithmetic(s,s->d[0],7,0,0); break;
    case AMIGA_MEM_ROUND_MASK_BYTE:
        s->d[0]&=~7u; flags(s,(uint8_t)s->d[0],0x80); break;
    case AMIGA_MEM_RESULT_ZERO: s->d[3]=0; flags(s,0,0x80000000u); break;
    case AMIGA_MEM_COMPARE_TOTAL: arithmetic(s,s->d[0],b->read32(b->context,s->a[0]+AMIGA_MEMORY_FREE),1,1); break;
    case AMIGA_MEM_BEGIN_FREE_LINK: s->a[2]=s->a[0]+AMIGA_MEMORY_FIRST; break;
    case AMIGA_MEM_NEXT_FREE_CHUNK: s->d[3]=b->read32(b->context,s->a[2]); flags(s,s->d[3],0x80000000u); break;
    case AMIGA_MEM_CHUNK_FROM_RESULT: s->a[1]=s->d[3]; break;
    case AMIGA_MEM_COMPARE_CHUNK_SIZE: arithmetic(s,s->d[0],b->read32(b->context,s->a[1]+AMIGA_CHUNK_BYTES),1,1); break;
    case AMIGA_MEM_FOLLOW_CHUNK: s->a[2]=s->a[1]; break;
    case AMIGA_MEM_SPLIT_ADDRESS: s->a[3]=s->a[1]+s->d[0]; break;
    case AMIGA_MEM_COPY_SPLIT_LINK: write_move(s,b,s->a[3],b->read32(b->context,s->a[1])); break;
    case AMIGA_MEM_LOAD_CHUNK_SIZE: s->d[3]=b->read32(b->context,s->a[1]+AMIGA_CHUNK_BYTES); flags(s,s->d[3],0x80000000u); break;
    case AMIGA_MEM_SUBTRACT_SIZE: s->d[3]=arithmetic(s,s->d[3],s->d[0],1,0); break;
    case AMIGA_MEM_STORE_SPLIT_SIZE: write_move(s,b,s->a[3]+AMIGA_CHUNK_BYTES,s->d[3]); break;
    case AMIGA_MEM_LINK_SPLIT: write_move(s,b,s->a[2],s->a[3]); break;
    case AMIGA_MEM_UNLINK_CHUNK: write_move(s,b,s->a[2],b->read32(b->context,s->a[1])); break;
    case AMIGA_MEM_SUBTRACT_TOTAL:
        a=s->a[0]+AMIGA_MEMORY_FREE; v=arithmetic(s,b->read32(b->context,a),s->d[0],1,0); b->write32(b->context,a,v); break;
    case AMIGA_MEM_RESULT_ADDRESS: s->d[3]=s->a[1]; flags(s,s->d[3],0x80000000u); break;
    case AMIGA_MEM_RETURN_RESULT: s->d[0]=s->d[3]; flags(s,s->d[0],0x80000000u); break;
    case AMIGA_MEM_ADDRESS_OFFSET: s->d[1]=s->a[1]; flags(s,s->d[1],0x80000000u); break;
    case AMIGA_MEM_ALIGNMENT_MASK: s->d[3]=0xFFFFFFF8u; flags(s,s->d[3],0x80000000u); break;
    case AMIGA_MEM_ALIGN_ADDRESS: s->d[1]&=s->d[3]; flags(s,s->d[1],0x80000000u); break;
    case AMIGA_MEM_EXCHANGE_ADDRESS: v=s->d[1]; s->d[1]=s->a[1]; s->a[1]=v; break;
    case AMIGA_MEM_ADDRESS_REMAINDER: s->d[1]=arithmetic(s,s->d[1],s->a[1],1,0); break;
    case AMIGA_MEM_ADD_REMAINDER: s->d[0]=arithmetic(s,s->d[0],s->d[1],0,0); break;
    case AMIGA_MEM_ALIGN_SIZE: s->d[0]&=s->d[3]; flags(s,s->d[0],0x80000000u); break;
    case AMIGA_MEM_COMPARE_FREE_ADDRESS: arithmetic(s,s->a[1],s->d[3],1,1); break;
    case AMIGA_MEM_FOLLOW_RESULT: s->a[2]=s->d[3]; break;
    case AMIGA_MEM_HEADER_LINK_OFFSET: s->d[1]=AMIGA_MEMORY_FIRST; flags(s,s->d[1],0x80000000u); break;
    case AMIGA_MEM_ADD_HEADER_ADDRESS: s->d[1]=arithmetic(s,s->d[1],s->a[0],0,0); break;
    case AMIGA_MEM_COMPARE_LINK_ADDRESS: arithmetic(s,s->d[1],s->a[2],1,1); break;
    case AMIGA_MEM_LOAD_LEFT_SIZE: s->d[3]=b->read32(b->context,s->a[2]+AMIGA_CHUNK_BYTES); flags(s,s->d[3],0x80000000u); break;
    case AMIGA_MEM_LEFT_END: s->d[3]=arithmetic(s,s->d[3],s->a[2],0,0); break;
    case AMIGA_MEM_COMPARE_LEFT_END: arithmetic(s,s->d[3],s->a[1],1,1); break;
    case AMIGA_MEM_COPY_LEFT_LINK: write_move(s,b,s->a[1],b->read32(b->context,s->a[2])); break;
    case AMIGA_MEM_LINK_FREED_CHUNK: write_move(s,b,s->a[2],s->a[1]); break;
    case AMIGA_MEM_STORE_FREED_SIZE: write_move(s,b,s->a[1]+AMIGA_CHUNK_BYTES,s->d[0]); break;
    case AMIGA_MEM_EXTEND_LEFT:
        a=s->a[2]+AMIGA_CHUNK_BYTES; v=arithmetic(s,b->read32(b->context,a),s->d[0],0,0); b->write32(b->context,a,v); break;
    case AMIGA_MEM_USE_LEFT_CHUNK: s->a[1]=s->a[2]; break;
    case AMIGA_MEM_TEST_NEXT_CHUNK: flags(s,b->read32(b->context,s->a[1]),0x80000000u); break;
    case AMIGA_MEM_CHUNK_END: s->d[3]=arithmetic(s,s->d[3],s->a[1],0,0); break;
    case AMIGA_MEM_COMPARE_RIGHT_ADDRESS: arithmetic(s,s->d[3],b->read32(b->context,s->a[1]),1,1); break;
    case AMIGA_MEM_USE_RIGHT_CHUNK: s->a[2]=b->read32(b->context,s->a[1]); break;
    case AMIGA_MEM_COPY_RIGHT_LINK: write_move(s,b,s->a[1],b->read32(b->context,s->a[2])); break;
    case AMIGA_MEM_EXTEND_RIGHT:
        a=s->a[1]+AMIGA_CHUNK_BYTES; v=arithmetic(s,b->read32(b->context,a),s->d[3],0,0); b->write32(b->context,a,v); break;
    case AMIGA_MEM_ADD_TOTAL:
        a=s->a[0]+AMIGA_MEMORY_FREE; v=arithmetic(s,b->read32(b->context,a),s->d[0],0,0); b->write32(b->context,a,v); break;
    case AMIGA_MEM_ALERT_NUMBER: s->d[7]=arg; flags(s,s->d[7],0x80000000u); break;
    case AMIGA_MEM_EXEC_BASE: s->a[6]=(uint32_t)(int32_t)b->read32(b->context,4); break;
    case AMIGA_MEM_TASK_DEPTH_UP:
        a=s->a[6]+AMIGA_EXEC_TD_NEST_CNT; old=b->read8(b->context,a); v=old+1;
        s->ccr=(uint8_t)((!(v&255)?AMIGA_CCR_Z:0)|(v&128?AMIGA_CCR_N:0)|
            (~(old^1)&(old^v)&128?AMIGA_CCR_V:0)|(v&256?AMIGA_CCR_C|AMIGA_CCR_X:0));
        b->write8(b->context,a,(uint8_t)v); break;
    case AMIGA_MEM_SAVE_SIZE: s->d[3]=s->d[0]; flags(s,s->d[3],0x80000000u); break;
    case AMIGA_MEM_SAVE_REQUIREMENTS: s->d[2]=s->d[1]; flags(s,s->d[2],0x80000000u); break;
    case AMIGA_MEM_BEGIN_HEADERS_A2: s->a[2]=s->a[6]+AMIGA_EXEC_MEMORY_LIST; break;
    case AMIGA_MEM_NEXT_HEADER_A2: s->a[2]=b->read32(b->context,s->a[2]); break;
    case AMIGA_MEM_TEST_HEADER_A2: flags(s,b->read32(b->context,s->a[2]),0x80000000u); break;
    case AMIGA_MEM_LOAD_ATTRIBUTES_A2: v=b->read16(b->context,s->a[2]+AMIGA_MEMORY_ATTRIBUTES); s->d[0]=(s->d[0]&0xFFFF0000u)|v; flags(s,v,0x8000); break;
    case AMIGA_MEM_MASK_ATTRIBUTES_D2: v=(uint16_t)(s->d[0]&s->d[2]); s->d[0]=(s->d[0]&0xFFFF0000u)|v; flags(s,v,0x8000); break;
    case AMIGA_MEM_COMPARE_ATTRIBUTES_D2:
        old=(uint16_t)s->d[0]; v=(uint16_t)s->d[2]; bit=(uint16_t)(old-v);
        s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!bit?AMIGA_CCR_Z:0)|(bit&0x8000?AMIGA_CCR_N:0)|
            ((old^v)&(old^bit)&0x8000?AMIGA_CCR_V:0)|(old<v?AMIGA_CCR_C:0)); break;
    case AMIGA_MEM_USE_HEADER_A2: s->a[0]=s->a[2]; break;
    case AMIGA_MEM_RESTORE_SIZE: s->d[0]=s->d[3]; flags(s,s->d[0],0x80000000u); break;
    case AMIGA_MEM_NO_ALLOCATION: s->d[0]=0; flags(s,0,0x80000000u); break;
    case AMIGA_MEM_TEST_CLEAR: s->ccr=(uint8_t)((s->ccr&~AMIGA_CCR_Z)|(!(s->d[2]&0x10000)?AMIGA_CCR_Z:0)); break;
    case AMIGA_MEM_CLEAR_VALUE: s->d[1]=0; flags(s,0,0x80000000u); break;
    case AMIGA_MEM_CLEAR_ROUND: s->d[3]=arithmetic(s,s->d[3],3,0,0); break;
    case AMIGA_MEM_CLEAR_WORD_COUNT:
        old=s->d[3]; s->d[3]>>=2; flags(s,s->d[3],0x80000000u);
        if (old&2) s->ccr|=AMIGA_CCR_X|AMIGA_CCR_C; else s->ccr&=~AMIGA_CCR_X; break;
    case AMIGA_MEM_CLEAR_ADDRESS: s->a[0]=s->d[0]; break;
    case AMIGA_MEM_CLEAR_WORD: write_move(s,b,s->a[0],s->d[1]); s->a[0]+=4; break;
    case AMIGA_MEM_NEXT_CLEAR_WORD:
        s->d[3]=(s->d[3]&0xFFFF0000u)|((s->d[3]-1)&0xFFFF); e->dbra_continues=(uint16_t)s->d[3]!=0xFFFF; break;
    case AMIGA_MEM_SWAP_WORD_COUNT: s->d[3]=(s->d[3]<<16)|(s->d[3]>>16); flags(s,s->d[3],0x80000000u); break;
    case AMIGA_MEM_TEST_WORD_COUNT: flags(s,(uint16_t)s->d[3],0x8000); break;
    case AMIGA_MEM_DECREMENT_WORD_COUNT:
        old=(uint16_t)s->d[3]; v=(uint16_t)(old-1); s->d[3]=(s->d[3]&0xFFFF0000u)|v;
        s->ccr=(uint8_t)((!v?AMIGA_CCR_Z:0)|(v&0x8000?AMIGA_CCR_N:0)|
            ((old^1)&(old^v)&0x8000?AMIGA_CCR_V:0)|(!old?AMIGA_CCR_X|AMIGA_CCR_C:0)); break;
    case AMIGA_MEM_BEGIN_HEADERS_A0: s->a[0]=s->a[6]+AMIGA_EXEC_MEMORY_LIST; break;
    case AMIGA_MEM_NEXT_HEADER_A0: s->a[0]=b->read32(b->context,s->a[0]); break;
    case AMIGA_MEM_TEST_HEADER_A0: flags(s,b->read32(b->context,s->a[0]),0x80000000u); break;
    case AMIGA_MEM_COMPARE_LOWER: arithmetic(s,s->a[1],b->read32(b->context,s->a[0]+AMIGA_MEMORY_LOWER),1,1); break;
    case AMIGA_MEM_COMPARE_UPPER: arithmetic(s,s->a[1],b->read32(b->context,s->a[0]+AMIGA_MEMORY_UPPER),1,1); break;
    case AMIGA_MEM_LOAD_ATTRIBUTES_A0: v=b->read16(b->context,s->a[0]+AMIGA_MEMORY_ATTRIBUTES); s->d[0]=(s->d[0]&0xFFFF0000u)|v; flags(s,v,0x8000); break;
    case AMIGA_MEM_BEGIN_HEADERS_A1: s->a[1]=s->a[6]+AMIGA_EXEC_MEMORY_LIST; break;
    case AMIGA_MEM_NEXT_HEADER_A1: s->a[1]=b->read32(b->context,s->a[1]); break;
    case AMIGA_MEM_TEST_HEADER_A1: flags(s,b->read32(b->context,s->a[1]),0x80000000u); break;
    case AMIGA_MEM_LOAD_ATTRIBUTES_A1: v=b->read16(b->context,s->a[1]+AMIGA_MEMORY_ATTRIBUTES); s->d[0]=(s->d[0]&0xFFFF0000u)|v; flags(s,v,0x8000); break;
    case AMIGA_MEM_MASK_ATTRIBUTES_D1: v=(uint16_t)(s->d[0]&s->d[1]); s->d[0]=(s->d[0]&0xFFFF0000u)|v; flags(s,v,0x8000); break;
    case AMIGA_MEM_COMPARE_ATTRIBUTES_D1:
        old=(uint16_t)s->d[0]; v=(uint16_t)s->d[1]; bit=(uint16_t)(old-v);
        s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!bit?AMIGA_CCR_Z:0)|(bit&0x8000?AMIGA_CCR_N:0)|
            ((old^v)&(old^bit)&0x8000?AMIGA_CCR_V:0)|(old<v?AMIGA_CCR_C:0)); break;
    case AMIGA_MEM_TEST_LARGEST: s->ccr=(uint8_t)((s->ccr&~AMIGA_CCR_Z)|(!(s->d[1]&0x20000)?AMIGA_CCR_Z:0)); break;
    case AMIGA_MEM_ACCUMULATE_FREE: s->d[3]=arithmetic(s,s->d[3],b->read32(b->context,s->a[1]+AMIGA_MEMORY_FREE),0,0); break;
    case AMIGA_MEM_FIRST_CHUNK_D0: s->d[0]=b->read32(b->context,s->a[1]+AMIGA_MEMORY_FIRST); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_MEM_CHUNK_FROM_D0: s->a[0]=s->d[0]; break;
    case AMIGA_MEM_COMPARE_LARGEST: arithmetic(s,s->d[3],b->read32(b->context,s->a[0]+AMIGA_CHUNK_BYTES),1,1); break;
    case AMIGA_MEM_REMEMBER_LARGEST: s->a[2]=s->a[0]; break;
    case AMIGA_MEM_LOAD_LARGEST: s->d[3]=b->read32(b->context,s->a[0]+AMIGA_CHUNK_BYTES); flags(s,s->d[3],0x80000000u); break;
    case AMIGA_MEM_NEXT_CHUNK_D0: s->d[0]=b->read32(b->context,s->a[0]); flags(s,s->d[0],0x80000000u); break;
    case AMIGA_MEM_ABSOLUTE_OFFSET: s->d[2]=s->a[1]; flags(s,s->d[2],0x80000000u); break;
    case AMIGA_MEM_MASK_ABSOLUTE_OFFSET: s->d[2]&=7; flags(s,s->d[2],0x80000000u); break;
    case AMIGA_MEM_ALIGN_ABSOLUTE_ADDRESS: s->a[1]-=s->d[2]; break;
    case AMIGA_MEM_ADD_ABSOLUTE_OFFSET: s->d[0]=arithmetic(s,s->d[0],s->d[2],0,0); break;
    case AMIGA_MEM_REMEMBER_ABSOLUTE_ADDRESS: s->a[3]=s->a[1]; break;
    case AMIGA_MEM_ABSOLUTE_END_START: s->d[2]=s->a[1]; flags(s,s->d[2],0x80000000u); break;
    case AMIGA_MEM_ABSOLUTE_END_SIZE: s->d[2]=arithmetic(s,s->d[2],s->d[0],0,0); break;
    case AMIGA_MEM_LOAD_ABSOLUTE_CHUNK_SIZE: s->d[4]=b->read32(b->context,s->a[1]+AMIGA_CHUNK_BYTES); flags(s,s->d[4],0x80000000u); break;
    case AMIGA_MEM_ABSOLUTE_CHUNK_END: s->d[4]=arithmetic(s,s->d[4],s->d[3],0,0); break;
    case AMIGA_MEM_COMPARE_ABSOLUTE_END: arithmetic(s,s->d[4],s->d[2],1,1); break;
    case AMIGA_MEM_COMPARE_ABSOLUTE_START: arithmetic(s,s->d[3],s->a[3],1,1); break;
    case AMIGA_MEM_ABSOLUTE_REMAINDER: s->d[4]=arithmetic(s,s->d[4],s->d[2],1,0); break;
    case AMIGA_MEM_ABSOLUTE_SUCCESSOR: s->a[0]=b->read32(b->context,s->a[1]); break;
    case AMIGA_MEM_ABSOLUTE_SPLIT_ADDRESS: s->a[0]=s->a[3]+s->d[0]; break;
    case AMIGA_MEM_COPY_ABSOLUTE_SPLIT_LINK: write_move(s,b,s->a[0],b->read32(b->context,s->a[1])); break;
    case AMIGA_MEM_LINK_ABSOLUTE_SPLIT: write_move(s,b,s->a[1],s->a[0]); break;
    case AMIGA_MEM_STORE_ABSOLUTE_SPLIT_SIZE: write_move(s,b,s->a[0]+AMIGA_CHUNK_BYTES,s->d[4]); break;
    case AMIGA_MEM_ABSOLUTE_LEFT_SIZE: s->d[3]=arithmetic(s,s->d[3],s->a[3],1,0); break;
    case AMIGA_MEM_NEGATE_LEFT_SIZE: s->d[3]=arithmetic(s,0,s->d[3],1,0); break;
    case AMIGA_MEM_STORE_ABSOLUTE_LEFT_SIZE: write_move(s,b,s->a[1]+AMIGA_CHUNK_BYTES,s->d[3]); break;
    case AMIGA_MEM_LINK_ABSOLUTE_SUCCESSOR: write_move(s,b,s->a[2],s->a[0]); break;
    case AMIGA_MEM_ABSOLUTE_RESULT: s->d[0]=s->a[3]; flags(s,s->d[0],0x80000000u); break;
    default: return 0;
    }
    return 1;
}
