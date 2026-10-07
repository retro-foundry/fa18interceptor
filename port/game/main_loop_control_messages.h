#ifndef FA18_MAIN_LOOP_CONTROL_MESSAGES_H
#define FA18_MAIN_LOOP_CONTROL_MESSAGES_H
#include "memory.h"
enum MainControlChild {
    MC_RESET_FACE_STATE,MC_CONTROL_TONE,MC_CONTROL_ALERT_FIRST,MC_CONTROL_ALERT_SECOND,
    MC_CONTROL_FALLBACK_TONE,MC_BEGIN_RECORD,MC_ADVANCE_RECORD,
    MC_SEQUENCE_RESTART_TONE,MC_ACCEPT_TYPED_CODE,MC_FINISH_SEQUENCE,MC_SEQUENCE_TONE,
    MC_GLYPH_FIRST,MC_GLYPH_SECOND,MC_GLYPH_THIRD,MC_GLYPH_FOURTH
};
/* Values reloaded from real child outputs. Cursors and glyph inputs remain
 * meaningful domain values; full register halves and CCR stay in the adapter. */
typedef struct {
    gaddr lookup,positions,text,glyph,origin,planes;
    uint32_t control,character,offset,style,colour;
} MessageWorking;
enum MainControlPhase {
    MC_PRIMARY_BYTE,MC_PRIMARY_WORD,MC_PRIMARY_LONG,MC_SECONDARY_BYTE,MC_SECONDARY_WORD,MC_SECONDARY_LONG,
    MC_CHARACTER_BYTE,MC_CHARACTER_LONG,MC_INDEX_BYTE,MC_INDEX_WORD,MC_STYLE_WORD,MC_COLOUR_WORD,MC_STRIDE_WORD,
    MC_LOOKUP,MC_POSITIONS,MC_TEXT,MC_GLYPH,MC_ORIGIN,MC_PLANES,
    MC_STORE_BYTE,MC_STORE_WORD,MC_STORE_LONG,MC_TEST_BYTE,MC_TEST_WORD,
    MC_COMPARE_BYTE,MC_COMPARE_WORD,MC_BIT_TEST,
    MC_PRIMARY_SUB_BYTE,MC_PRIMARY_SUB_WORD,MC_PRIMARY_ADD_LONG,MC_PRIMARY_AND_BYTE,MC_PRIMARY_AND_WORD,MC_PRIMARY_OR_BYTE,MC_PRIMARY_OR_WORD,
    MC_PRIMARY_EXT_WORD,MC_PRIMARY_EXT_LONG,MC_PRIMARY_SHIFT_WORD,MC_PRIMARY_SHIFT_LONG,MC_PRIMARY_DOUBLE,MC_PRIMARY_ADD_SECONDARY,
    MC_SECONDARY_AND_BYTE,MC_SECONDARY_AND_WORD,MC_SECONDARY_OR_BYTE,MC_SECONDARY_ADD_WORD,MC_SECONDARY_SUB_BYTE,MC_SECONDARY_EXT_WORD,
    MC_CHARACTER_AND_WORD,MC_CHARACTER_SUB_WORD,MC_CHARACTER_DOUBLE,MC_INDEX_EXT_WORD,MC_INDEX_INCREMENT,
    MC_OFFSET_EXT_LONG,MC_OFFSET_ADD_LONG,MC_COLOUR_FROM_CONTROL,
    MC_MEMORY_ADD_BYTE,MC_MEMORY_SUB_BYTE,MC_MEMORY_ADD_WORD,MC_MEMORY_SUB_WORD,
    MC_MESSAGE_CLEAR_BEGIN,MC_MESSAGE_CLEAR_BYTE,MC_MESSAGE_CLEAR_DONE,
    MC_CURSOR_LOAD,MC_CURSOR_STORE,MC_SELECT_MESSAGE,
    MC_GLYPH_SELECT,MC_PLANE_ARGUMENT,MC_PLANE_STYLE
};
typedef struct {
    MessageWorking (*consume)(void *context,enum MainControlChild child);
    void (*observe)(void *context,enum MainControlPhase phase,uint32_t value,uint32_t other);
    void *context;
    /* Native callers pass game values directly, with no register observer. */
    MessageWorking (*consume_values)(void *context,enum MainControlChild child,MessageWorking work);
} MainControlHooks;
void advance_main_loop_control_records(gaddr frame,const MainControlHooks *h);
/* C32CEE can assign a character or glyph address consumed by the following
 * pending-input parent. Only the low byte is part of that publication contract.
 * Delay/no-message exits preserve the caller's value rather than define zero. */
typedef struct { uint8_t input_byte; int assigned; } MessageSequenceResult;
MessageSequenceResult advance_main_loop_message_sequence(MessageWorking work,const MainControlHooks *h);
#endif
