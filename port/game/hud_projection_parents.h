#ifndef FA18_HUD_PROJECTION_PARENTS_H
#define FA18_HUD_PROJECTION_PARENTS_H
#include "memory.h"
enum HudProjectionField { HPR_VALUE,HPR_BASE,HPR_CONTROL,HPR_SECONDARY,HPR_SOURCE,HPR_STRIDE,HPR_SIZE,HPR_OFFSET,
    HPR_REGISTERS,HPR_TABLE,HPR_STREAM,HPR_DESCRIPTOR,HPR_SCREEN,HPR_MODULO,HPR_FRAME,HPR_STACK };
typedef struct {
    uint32_t value,base,control,secondary,source,stride,size,offset;
    gaddr registers,table,stream,descriptor,screen,modulo,frame,stack;
    int less;
} HudProjectionState;
enum HudProjectionChild { HPR_FIXED_CIRCLE,HPR_MAGNITUDE,HPR_SCALED_CIRCLE,HPR_RECORD_POINT,HPR_SHOOT,HPR_POINT,HPR_RING,HPR_RANGE,HPR_RATE,HPR_TRANSFORM,HPR_SETUP,HPR_CENTRE,HPR_PITCH,HPR_ROLL,HPR_STATUS,HPR_CLOCK,HPR_DISPLAY,HPR_CUE,HPR_SMALL_GLYPH,HPR_FAULT };
enum HudProjectionPhase {
    HPR_WORD,HPR_BYTE,HPR_LONG,HPR_POINTER,HPR_ADD_WORD,HPR_SUB_WORD,HPR_ADD_LONG,HPR_SUB_LONG,HPR_NEG_WORD,
    HPR_SWAP,HPR_EXT_WORD,HPR_EXT_LONG,HPR_ASL_WORD,HPR_LSL_WORD,HPR_ASR_WORD,HPR_ASR_BYTE,HPR_LSR_LONG,HPR_ROR_WORD,
    HPR_AND_WORD,HPR_AND_BYTE,HPR_OR_WORD,HPR_COMPARE_WORD,HPR_COMPARE_BYTE,HPR_TEST_WORD,HPR_TEST_BYTE,
    HPR_BIT_TEST,HPR_BIT_CLEAR,HPR_STORE_BYTE,HPR_STORE_WORD,HPR_STORE_LONG,HPR_MEMORY_SUB_BYTE,
    HPR_DECREMENT,HPR_PUSH_LONG,HPR_POP_LONG,HPR_SAVE_WORDS,HPR_RESTORE_WORDS,HPR_ASR_LONG,HPR_LSR_BYTE,HPR_DIVU,HPR_DIVS,HPR_AND_LONG,HPR_COMPARE_LONG,HPR_MEMORY_LOGIC_LONG,HPR_SAVE_LONGS,HPR_RESTORE_LONGS,HPR_PUSH_WORD,HPR_POP_WORD,HPR_RAW_LONG,HPR_MULS,HPR_MULU,HPR_POP_POINTER
};
typedef struct {
    HudProjectionState (*consume)(void *context,enum HudProjectionChild child);
    void (*observe)(void *context,enum HudProjectionPhase phase,enum HudProjectionField field,uint32_t value,uint32_t operand);
    HudProjectionState (*restored)(void *context);
    void *context;
} HudProjectionHooks;
void hud_fixed_mark(HudProjectionState w,const HudProjectionHooks *h);
void hud_scaled_circle(HudProjectionState w,const HudProjectionHooks *h);
void hud_record_transform(HudProjectionState w,const HudProjectionHooks *h);
void hud_weapon_cue(HudProjectionState w,const HudProjectionHooks *h);
void hud_postflight_outer(HudProjectionState w,const HudProjectionHooks *h);
void hud_conditional_text(HudProjectionState w,const HudProjectionHooks *h);
#endif
