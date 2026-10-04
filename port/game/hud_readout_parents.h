#ifndef FA18_HUD_READOUT_PARENTS_H
#define FA18_HUD_READOUT_PARENTS_H
#include "memory.h"
enum HudReadoutParentField { HRP_VALUE,HRP_BASE,HRP_CONTROL,HRP_SECONDARY,HRP_SOURCE,HRP_STRIDE,HRP_SIZE,HRP_OFFSET,
    HRP_REGISTERS,HRP_TABLE,HRP_STREAM,HRP_DESCRIPTOR,HRP_SCREEN,HRP_MODULO };
typedef struct {
    uint32_t value,base,control,secondary,source,stride,size,offset;
    gaddr registers,table,stream,descriptor,screen,modulo;
    int less;
} HudReadoutParentState;
enum HudReadoutParentChild {
    HRP_CALL_C32804,
    HRP_CALL_C31CFE,
    HRP_CALL_C31D4A,
    HRP_CALL_C31E50,
    HRP_CALL_C31EFE,
    HRP_CALL_C31FA8,
    HRP_CALL_C320C2,
    HRP_CALL_C32158,
    HRP_CALL_C321B2,
    HRP_CALL_C3222E,
    HRP_CALL_C322BC,
    HRP_CALL_C31D98,
    HRP_CALL_C31DD6,
    HRP_CALL_C31F90,
    HRP_CALL_C32146,
    HRP_CALL_C321A0,
    HRP_CALL_C3200A,
    HRP_CALL_C3211A,
    HRP_CALL_C31F38,
    HRP_CALL_C31F48,
    HRP_CALL_C32252,
    HRP_CALL_C322E0,
    HRP_CALL_C32982,
    HRP_CALL_C32A14,
    HRP_CALL_C327EE,
    HRP_CALL_C31D14,
    HRP_CALL_C31E5C,
    HRP_CALL_C31D62,
    HRP_CALL_C33FB0,
    HRP_CALL_C31DC8,
    HRP_CALL_C31EB2
};
enum HudReadoutParentPhase {
    HRP_WORD,HRP_BYTE,HRP_LONG,HRP_POINTER,HRP_ADD_WORD,HRP_SUB_WORD,HRP_ADD_LONG,HRP_SUB_LONG,HRP_NEG_WORD,
    HRP_SWAP,HRP_EXT_WORD,HRP_EXT_LONG,HRP_ASL_WORD,HRP_LSL_WORD,HRP_ASR_WORD,HRP_ASR_BYTE,HRP_LSR_LONG,HRP_ROR_WORD,
    HRP_AND_WORD,HRP_AND_BYTE,HRP_OR_WORD,HRP_COMPARE_WORD,HRP_COMPARE_BYTE,HRP_TEST_WORD,HRP_TEST_BYTE,
    HRP_BIT_TEST,HRP_BIT_CLEAR,HRP_STORE_BYTE,HRP_STORE_WORD,HRP_STORE_LONG,HRP_MEMORY_SUB_BYTE,
    HRP_DECREMENT,HRP_PUSH_LONG,HRP_POP_LONG,HRP_SAVE_WORDS,HRP_RESTORE_WORDS,HRP_ASR_LONG,HRP_LSR_BYTE,HRP_DIVU,HRP_DIVS,HRP_AND_LONG,HRP_COMPARE_LONG,HRP_MEMORY_LOGIC_LONG,HRP_SAVE_LONGS,HRP_RESTORE_LONGS,HRP_PUSH_WORD,HRP_POP_WORD
};
typedef struct {
    HudReadoutParentState (*consume)(void *context,enum HudReadoutParentChild child);
    void (*observe)(void *context,enum HudReadoutParentPhase phase,enum HudReadoutParentField field,uint32_t value,uint32_t operand);
    HudReadoutParentState (*restored)(void *context);
    void *context;
} HudReadoutParentHooks;
void hud_readout_C31F4C(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C3201A(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C3212A(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C32178(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C321D2(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C32260(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C31EB6(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C31C60(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C31D16(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C31E6C(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C31D64(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C33F54(HudReadoutParentState w,const HudReadoutParentHooks *h);
void hud_readout_C328A8(HudReadoutParentState w,const HudReadoutParentHooks *h);
#endif
