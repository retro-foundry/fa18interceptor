#ifndef FA18_TARGET_HEADING_H
#define FA18_TARGET_HEADING_H
#include "memory.h"
enum MenuHeadingPhase {
    MH_LIST_START, MH_LIST_TEST, MH_RECORD_OFFSET, MH_TYPE, MH_TYPE_MASK,
    MH_TYPE_COMPARE, MH_RECORD_FLAG, MH_RECORD_SELECTED, MH_WORLD_INPUT,
    MH_DIRECTION, MH_DIVIDE, MH_PACKED, MH_NIBBLE_CLEAR, MH_DIGIT_MASK,
    MH_DIGIT_COMPARE, MH_ROUND_BEGIN, MH_DECIMAL, MH_ROUND_COMPARE,
    MH_WORD_CLEAR, MH_DIGIT_LOAD, MH_DIGIT_SHIFT, MH_ASCII, MH_CHARACTER,
    MH_COMPLETE, MH_NOT_FOUND
};
typedef struct {
    void (*observe)(void *context,enum MenuHeadingPhase phase,uint32_t value,gaddr address);
    void (*world)(void *context,gaddr record,uint32_t result[3]);
    void (*track)(void *context,uint32_t x,uint32_t z);
    void (*pack)(void *context);
    void *context;
} MenuHeadingHooks;
int refresh_menu_heading(const MenuHeadingHooks *hooks);

/* $C25070: find the first active class-$10 record in the post-input list,
 * aim the tracked heading at a point through it, and write three heading
 * characters. Returns -1 when the list has no matching record, 0 otherwise. */
int refresh_post_input_heading(void);

#endif
