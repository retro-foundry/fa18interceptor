#ifndef FA18_HUD_TEXT_HELPERS_H
#define FA18_HUD_TEXT_HELPERS_H
#include "memory.h"
enum HudTextField { HTH_VALUE,HTH_BASE,HTH_CONTROL,HTH_SECONDARY,HTH_SOURCE,HTH_STRIDE,HTH_SIZE,HTH_OFFSET,
    HTH_REGISTERS,HTH_TABLE,HTH_STREAM,HTH_DESCRIPTOR,HTH_SCREEN,HTH_MODULO };
typedef struct {
    uint32_t value,base,control,secondary,source,stride,size,offset;
    gaddr registers,table,stream,descriptor,screen,modulo;
    int less;
} HudTextState;
enum HudTextChild { HTH_FAULT,HTH_SMALL_GLYPH,HTH_VIEW_FIRST,HTH_VIEW_SECOND,HTH_VIEW_THIRD,HTH_VIEW_FOURTH };
enum HudTextPhase {
    HTH_WORD,HTH_BYTE,HTH_LONG,HTH_POINTER,HTH_ADD_WORD,HTH_SUB_WORD,HTH_ADD_LONG,HTH_SUB_LONG,HTH_NEG_WORD,
    HTH_SWAP,HTH_EXT_WORD,HTH_EXT_LONG,HTH_ASL_WORD,HTH_LSL_WORD,HTH_ASR_WORD,HTH_ASR_BYTE,HTH_LSR_LONG,HTH_ROR_WORD,
    HTH_AND_WORD,HTH_AND_BYTE,HTH_OR_WORD,HTH_COMPARE_WORD,HTH_COMPARE_BYTE,HTH_TEST_WORD,HTH_TEST_BYTE,
    HTH_BIT_TEST,HTH_BIT_CLEAR,HTH_STORE_BYTE,HTH_STORE_WORD,HTH_STORE_LONG,HTH_MEMORY_SUB_BYTE,
    HTH_DECREMENT,HTH_PUSH_LONG,HTH_POP_LONG,HTH_SAVE_WORDS,HTH_RESTORE_WORDS,HTH_ASR_LONG,HTH_LSR_BYTE,HTH_DIVU,HTH_DIVS,HTH_AND_LONG,HTH_COMPARE_LONG,HTH_MEMORY_LOGIC_LONG,HTH_SAVE_LONGS,HTH_RESTORE_LONGS,HTH_PUSH_WORD,HTH_POP_WORD
};
typedef struct {
    HudTextState (*consume)(void *context,enum HudTextChild child);
    void (*observe)(void *context,enum HudTextPhase phase,enum HudTextField field,uint32_t value,uint32_t operand);
    HudTextState (*restored)(void *context);
    void *context;
} HudTextHooks;
void hud_text_cache(HudTextState w,const HudTextHooks *h);
void hud_text_small_hex(HudTextState w,const HudTextHooks *h);
void hud_text_small_fixed(HudTextState w,const HudTextHooks *h);
void hud_text_small_inverse(HudTextState w,const HudTextHooks *h);
void hud_text_small_line(HudTextState w,const HudTextHooks *h);
void hud_text_view_digits(HudTextState w,const HudTextHooks *h,int clear_leading);
void hud_text_view_line(HudTextState w,const HudTextHooks *h);
#endif
