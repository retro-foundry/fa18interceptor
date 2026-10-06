#ifndef FA18_MENU_SETUP_H
#define FA18_MENU_SETUP_H
#include "memory.h"
enum MenuSetupCall {
    MENU_SETUP_SOUND, MENU_SETUP_CLEAR, MENU_SETUP_RESET, MENU_SETUP_DELAY, MENU_SETUP_SCRIPT,
    MENU_SOUND_FREE_BEFORE, MENU_SOUND_FIXED_FIRST, MENU_SOUND_FIXED_SECOND,
    MENU_SOUND_ARGUMENT_FIRST, MENU_SOUND_ARGUMENT_SECOND, MENU_SOUND_FREE_OTHER
};
enum MenuSetupPhase {
    MENU_SETUP_BYTE_TEST, MENU_SETUP_BYTE_D0, MENU_SETUP_BYTE_STORE,
    MENU_SETUP_WORD_STORE, MENU_SETUP_LONG_STORE, MENU_SETUP_BIT_TEST,
    MENU_SETUP_CURSOR, MENU_SETUP_QUEUE_WORD, MENU_SETUP_QUEUE_NEXT, MENU_SETUP_CALLBACK,
    MENU_SETUP_TAIL_CALLBACK, MENU_SETUP_COCKPIT_READ, MENU_SETUP_MESSAGE_READ,
    MENU_SETUP_COCKPIT_HELD, MENU_SETUP_MESSAGE_MASK, MENU_SETUP_MESSAGE_COMPARE,
    MENU_SETUP_MESSAGE_STATE, MENU_SETUP_SELECTOR_BASE, MENU_SETUP_SELECTOR_MODE,
    MENU_SETUP_SELECTOR_ROW, MENU_SETUP_SELECTOR_CODE, MENU_SETUP_SELECTOR_DESTINATION
};
typedef struct {
    void (*consume)(void *context,enum MenuSetupCall call,uint32_t value);
    void (*observe)(void *context,enum MenuSetupPhase phase,uint32_t value,gaddr address);
    void *context;
} MenuSetupHooks;
void start_top_level_menu(const MenuSetupHooks *hooks);
/* C0FBE0 split at C0E78A's busy delay for a yielding host frontend. */
void begin_top_level_menu(const MenuSetupHooks *hooks);
void finish_top_level_menu(const MenuSetupHooks *hooks);
/* Shared C0FBE0 message publication, also used by the native frontend. */
void queue_top_level_menu_messages(const MenuSetupHooks *hooks);
void select_menu_sound_pair(uint32_t volume,const MenuSetupHooks *hooks);
void begin_menu_countdown(const MenuSetupHooks *hooks);
uint16_t filter_cockpit_message(uint16_t code,const MenuSetupHooks *hooks);
void queue_indexed_menu_message(uint32_t mode,uint32_t position,uint32_t row,const MenuSetupHooks *hooks);
#endif
