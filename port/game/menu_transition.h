#ifndef FA18_MENU_TRANSITION_H
#define FA18_MENU_TRANSITION_H
#include "memory.h"

enum MenuTransitionCall {
    MENU_PHASE_RESET, MENU_POSITIVE_RESET, MENU_POSITIVE_CLEAR,
    MENU_NEGATIVE_RESET, MENU_NEGATIVE_CLEAR, MENU_OTHER_CLEAR, MENU_SUMMARY,
    MENU_STOP_ZERO, MENU_STOP_ONE, MENU_SOUND_PAIR, MENU_NOISE, MENU_SCRIPTED_NOISE,
    MENU_DELAY_RESET, MENU_DELAY_ROOT, MENU_DELAY_VIEWPORT,
    MENU_MODE_ONE_ROOT, MENU_MODE_TWO_ROOT, MENU_MODE_RESTORE,
    MENU_MODE_RESTORE_STATE, MENU_MODE_RESTORE_POSITION, MENU_MODE_RESTORE_ROOT,
    MENU_MODE_NINE_ROOT, MENU_MODE_NINE_POSITION, MENU_MODE_NINE_RESET,
    MENU_MODE_NINE_VIEW, MENU_REFRESH,
    MENU_PAIR_FIRST, MENU_PAIR_SECOND,
    MENU_FIELD_FIRST, MENU_FIELD_SECOND, MENU_FIELD_THIRD, MENU_FIELD_FOURTH,
    MENU_FIELD_FIFTH, MENU_FIELD_SIXTH, MENU_FIELD_SEVENTH, MENU_FIELD_EIGHTH,
    MENU_FIELD_HOURS, MENU_FIELD_MINUTES, MENU_FIELD_NINTH, MENU_FIELD_LAST
};
enum MenuTransitionPhase {
    MENU_BYTE_TEST, MENU_WORD_TEST, MENU_BYTE_STORE, MENU_WORD_STORE, MENU_LONG_STORE,
    MENU_BYTE_D0, MENU_WORD_D0, MENU_LONG_D0, MENU_FULL_D0, MENU_FULL_D1,
    MENU_COMPARE_BYTE, MENU_COMPARE_WORD, MENU_BIT_TEST,
    MENU_FOLLOW_LOCALS, MENU_DELAY_LOCALS, MENU_PHASE_SUBTRACT,
    MENU_QUEUE_WORD, MENU_QUEUE_ADVANCE, MENU_CALLBACK,
    MENU_TABLE_BEGIN, MENU_TABLE_NEXT, MENU_TABLE_COMPARE,
    MENU_POSE_SUBTRACT, MENU_REQUEST_WORD,
    MENU_FORMAT_BEGIN, MENU_FORMAT_FIELD, MENU_DIVIDE, MENU_SAVE_TIME, MENU_RESTORE_TIME,
    MENU_EXTEND_TIME, MENU_SWAP_TIME
};
typedef struct { uint32_t value; gaddr record; } MenuTransitionResult;
typedef struct {
    MenuTransitionResult (*consume)(void *context,enum MenuTransitionCall call,uint32_t value);
    void (*observe)(void *context,enum MenuTransitionPhase phase,uint32_t value,uint32_t extra,gaddr address);
    void *context;
} MenuTransitionHooks;

void follow_top_level_menu(const MenuTransitionHooks *hooks);
void advance_delayed_menu(const MenuTransitionHooks *hooks);
void enter_menu_mode_nine(const MenuTransitionHooks *hooks);
void enter_menu_demonstration(const MenuTransitionHooks *hooks);
void start_menu_alert_pair(uint32_t argument,const MenuTransitionHooks *hooks);
void format_menu_summary(const MenuTransitionHooks *hooks);
#endif
