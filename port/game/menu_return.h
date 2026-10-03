#ifndef FA18_MENU_RETURN_H
#define FA18_MENU_RETURN_H
#include "memory.h"
enum MenuReturnChild { MR_CHOOSE_RESET, MR_LEAVE_RESET, MR_MESSAGE_CANCEL,
    MR_MESSAGE_RESET, MR_CONTEXT_CANCEL, MR_SMOOTH_CANCEL, MR_END_CANCEL,
    MR_SELECT_KEY, MR_SELECT_RESET, MR_CANCEL_REFRESH, MR_CANCEL_RESET };
enum MenuReturnPhase { MR_BYTE_TEST, MR_BYTE_STORE, MR_WORD_STORE,
    MR_BYTE_D0, MR_WORD_D0, MR_FULL_D0, MR_CALLBACK, MR_SUBTRACT_BYTE,
    MR_COMPARE_BYTE, MR_COMPARE_LONG, MR_SIGNED_MODE, MR_VIEWPORT_COMPARE,
    MR_CURSOR_INIT, MR_QUEUE_CODE, MR_CURSOR_NEXT };
typedef struct {
    void (*consume)(void *context,enum MenuReturnChild child);
    void (*observe)(void *context,enum MenuReturnPhase phase,uint32_t value,gaddr address);
    void *context;
} MenuReturnHooks;
void finish_menu_context_three(const MenuReturnHooks *hooks);
void follow_menu_return_message(const MenuReturnHooks *hooks);
void follow_menu_return_context(const MenuReturnHooks *hooks);
void begin_menu_context_ready(const MenuReturnHooks *hooks);
void choose_menu_exit_after_countdown(const MenuReturnHooks *hooks);
void leave_menu_on_key_or_message(const MenuReturnHooks *hooks);
void reset_menu_viewport_after_countdown(const MenuReturnHooks *hooks);
void enter_menu_mode_four(const MenuReturnHooks *hooks);
void start_menu_smoothing(const MenuReturnHooks *hooks);
void complete_menu_return_after_countdown(const MenuReturnHooks *hooks);
void select_menu_return_message(const MenuReturnHooks *hooks);
void cancel_menu_return(const MenuReturnHooks *hooks);
void leave_menu_return_on_key(const MenuReturnHooks *hooks);
#endif
