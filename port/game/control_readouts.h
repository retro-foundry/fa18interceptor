#ifndef FA18_CONTROL_READOUTS_H
#define FA18_CONTROL_READOUTS_H
#include "memory.h"
enum ControlReadoutField {
 CR_VALUE,CR_AMOUNT,CR_SCRATCH,CR_COUNTER,CR_DIVIDEND_SIGN,CR_DIVISOR_SIGN,
 CR_SPARE,CR_RESERVED,CR_RECORD,CR_LOOKUP,CR_STREAM,CR_OUTPUT,CR_TARGET,CR_BASE,CR_FRAME,CR_STACK
};
typedef struct {
 uint32_t value,amount,scratch,counter,dividend_sign,divisor_sign,spare,reserved;
 gaddr record,lookup,stream,output,target,base,frame,stack;
 unsigned extend;
} ControlReadoutState;
enum ControlReadoutPhase {
 CR_BYTE,CR_WORD,CR_LONG,CR_POINTER,CR_STORE_BYTE,CR_STORE_WORD,CR_STORE_LONG,
 CR_ADD_BYTE,CR_ADD_WORD,CR_ADD_LONG,CR_SUB_BYTE,CR_SUB_WORD,CR_SUB_LONG,
 CR_AND_BYTE,CR_OR_BYTE,CR_XOR_LONG,CR_ASR_WORD,CR_ASR_LONG,CR_ASL_WORD,CR_ASL_LONG,
 CR_EXT_WORD,CR_EXT_LONG,CR_COMPARE_BYTE,CR_COMPARE_WORD,CR_COMPARE_LONG,
 CR_TEST_BYTE,CR_TEST_WORD,CR_TEST_LONG,CR_BIT_TEST,CR_NEGATE_MEMORY_WORD,
 CR_ADD_MEMORY_WORD,CR_SUB_MEMORY_WORD,CR_LINK,CR_UNLINK,CR_SAVE_LONGS,CR_RESTORE_LONGS,
 CR_PUSH_LONG,CR_STACK_ADD,CR_NEG_LONG,CR_ROXL_LONG,CR_DBRA
};
typedef struct {
 ControlReadoutState (*consume)(void *,gaddr original_return);
 void (*observe)(void *,enum ControlReadoutPhase,enum ControlReadoutField,uint32_t,uint32_t);
 ControlReadoutState (*restored)(void *);
 void *context;
} ControlReadoutHooks;
void update_control_record_readouts(ControlReadoutState,const ControlReadoutHooks *);
void scale_control_record_magnitude(ControlReadoutState,const ControlReadoutHooks *);
void play_control_record_event(ControlReadoutState,const ControlReadoutHooks *);
void read_control_record_step(ControlReadoutState,const ControlReadoutHooks *);
void scale_control_five_eighths(ControlReadoutState,const ControlReadoutHooks *);
void divide_control_long(ControlReadoutState,const ControlReadoutHooks *);
#endif
