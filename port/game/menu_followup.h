#ifndef FA18_MENU_FOLLOWUP_H
#define FA18_MENU_FOLLOWUP_H
#include "memory.h"
enum MenuFollowupChild { MF_RESET, MF_FILE_RELEASE, MF_FILE_CHECK,
    MF_FILE_YIELD_BEFORE_OPEN, MF_FILE_OPEN, MF_FILE_YIELD_AFTER_OPEN,
    MF_FILE_READ, MF_FILE_YIELD_AFTER_READ, MF_FILE_CLOSE,
    MF_FILE_YIELD_AFTER_CLOSE, MF_FILE_OWN };
enum MenuFollowupPhase { MF_BYTE_TEST, MF_BYTE_STORE, MF_WORD_STORE,
    MF_WORD_D0, MF_CALLBACK, MF_VIEWPORT_COMPARE, MF_ONE_D0,
    MF_LATCH, MF_SEQUENCE_D0, MF_QUEUE_START, MF_QUEUE_OFF,
    MF_MODE_COMPARE, MF_MODE_SUBTRACT, MF_QUEUE_CODE, MF_QUEUE_NEXT,
    MF_FILE_STATUS, MF_FILE_HANDLE, MF_FILE_HANDLE_TEST,
    MF_FILE_READ_RESULT, MF_FILE_COMPARE, MF_FILE_ZERO };
typedef struct {
    uint32_t (*consume)(void *context,enum MenuFollowupChild child,uint32_t value,gaddr address);
    void (*observe)(void *context,enum MenuFollowupPhase phase,uint32_t value,gaddr address);
    void *context;
} MenuFollowupHooks;
void poll_menu_viewport(const MenuFollowupHooks *hooks,int alternate);
void follow_menu_key_or_countdown(const MenuFollowupHooks *hooks);
void advance_menu_mode_messages(const MenuFollowupHooks *hooks);
void load_menu_mode_file(const MenuFollowupHooks *hooks);
#endif
