#ifndef FA18_HUD_PARENTS_H
#define FA18_HUD_PARENTS_H
#include "memory.h"
enum HudParentField { HP_VALUE,HP_BASE,HP_CONTROL,HP_SECONDARY,HP_SOURCE,HP_STRIDE,HP_SIZE,HP_OFFSET,
    HP_REGISTERS,HP_TABLE,HP_STREAM,HP_DESCRIPTOR,HP_SCREEN,HP_MODULO };
typedef struct {
    uint32_t value,base,control,secondary,source,stride,size,offset;
    gaddr registers,table,stream,descriptor,screen,modulo;
    int less;
} HudParentState;
enum HudParentChild {
    HP_COUNTER_CLIP,HP_COUNTER_WAIT,HP_COUNTER_STORE,HP_COUNTER_TABLE_FIRST,HP_COUNTER_TABLE_SECOND,HP_COUNTER_TABLE_THIRD,
    HP_COUNTER_WAIT_OTHER,HP_COUNTER_STORE_OTHER,HP_COUNTER_NEXT_FIRST,HP_COUNTER_NEXT_SECOND,HP_COUNTER_NEXT_THIRD,
    HP_TABLE_RENDER,HP_STATUS_LINE,HP_STATUS_CLIP_FIRST,HP_STATUS_BLIT_FIRST,HP_STATUS_CLIP_SECOND,HP_STATUS_BLIT_SECOND,HP_STATUS_CLIP_THIRD,HP_STATUS_WAIT,
    HP_RECORD_CLIP_FIRST,HP_RECORD_BLIT,HP_RECORD_CLIP_SECOND,HP_RECORD_WAIT,HP_RECORD_LINE,
    HP_CACHED_UPDATE,HP_CACHED_CLIP,HP_CACHED_WAIT,HP_CACHED_LINE,
    HP_BITS_POINT,HP_BITS_FIRST,HP_BITS_SECOND,HP_BITS_THIRD,HP_BITS_FOURTH,HP_BITS_FIFTH,HP_BITS_SIXTH,HP_BITS_SEVENTH,HP_BITS_EIGHTH,HP_BITS_NINTH,
    HP_CLASS_BCD,HP_SCALE_POINT,HP_SCALE_BCD,HP_GLYPH,HP_FAULT
};
enum HudParentPhase {
    HP_WORD,HP_BYTE,HP_LONG,HP_POINTER,HP_ADD_WORD,HP_SUB_WORD,HP_ADD_LONG,HP_SUB_LONG,HP_NEG_WORD,
    HP_SWAP,HP_EXT_WORD,HP_EXT_LONG,HP_ASL_WORD,HP_LSL_WORD,HP_ASR_WORD,HP_ASR_BYTE,HP_LSR_LONG,HP_ROR_WORD,
    HP_AND_WORD,HP_AND_BYTE,HP_OR_WORD,HP_COMPARE_WORD,HP_COMPARE_BYTE,HP_TEST_WORD,HP_TEST_BYTE,
    HP_BIT_TEST,HP_BIT_CLEAR,HP_STORE_BYTE,HP_STORE_WORD,HP_STORE_LONG,HP_MEMORY_SUB_BYTE,
    HP_DECREMENT,HP_PUSH_LONG,HP_POP_LONG,HP_SAVE_WORDS,HP_RESTORE_WORDS
};
typedef struct {
    HudParentState (*consume)(void *context,enum HudParentChild child);
    void (*observe)(void *context,enum HudParentPhase phase,enum HudParentField field,uint32_t value,uint32_t operand);
    HudParentState (*restored)(void *context);
    void *context;
} HudParentHooks;
void draw_counter_stream_display(HudParentState w,const HudParentHooks *h);
void draw_table_stream_display(HudParentState w,const HudParentHooks *h);
void draw_status_stream_display(HudParentState w,const HudParentHooks *h);
void draw_record_stream_display(HudParentState w,const HudParentHooks *h);
void draw_cached_stream_display(HudParentState w,const HudParentHooks *h);
void draw_bit_selected_points(HudParentState w,const HudParentHooks *h);
void draw_record_class_digits(HudParentState w,const HudParentHooks *h);
void draw_record_scale_digits(HudParentState w,const HudParentHooks *h);
#endif
