#ifndef FA18_MENU_OUTCOME_H
#define FA18_MENU_OUTCOME_H
#include "memory.h"
enum MenuOutcomeChild { MO_DELAYED_RESET, MO_DELAYED_MESSAGE_DISABLED,
    MO_DELAYED_MESSAGE_ENABLED, MO_PAUSE_SCENE, MO_OUTCOME_MESSAGE,
    MO_OUTCOME_RESET, MO_MESSAGE_RESET, MO_COUNTDOWN_RESET };
enum MenuOutcomePhase { MO_BYTE_TEST, MO_BYTE_STORE, MO_WORD_STORE,
    MO_BYTE_D0, MO_WORD_D0, MO_FULL_D0, MO_CALLBACK, MO_LATCH,
    MO_MODE_LOCAL, MO_MODE_COMPARE, MO_SELECTED_MODE, MO_CONTEXT_MODE,
    MO_SEARCH_BEGIN, MO_SEARCH_SUBTRACT, MO_SEARCH_COMPARE,
    MO_TABLE_READ, MO_TABLE_ADD, MO_TABLE_STORE, MO_ATTEMPT_SUBTRACT,
    MO_MESSAGE_SUBTRACT };
typedef struct {
    void (*consume)(void *context,enum MenuOutcomeChild child,uint32_t value);
    void (*observe)(void *context,enum MenuOutcomePhase phase,uint32_t value,gaddr address);
    void *context;
} MenuOutcomeHooks;
void select_delayed_menu_message(const MenuOutcomeHooks *hooks);
void pause_menu_after_countdown(const MenuOutcomeHooks *hooks);
void queue_menu_message_four(const MenuOutcomeHooks *hooks);
void finish_menu_outcome(const MenuOutcomeHooks *hooks);
void leave_delayed_menu_message(const MenuOutcomeHooks *hooks);
void start_menu_context_after_countdown(const MenuOutcomeHooks *hooks);
void start_menu_outcome(const MenuOutcomeHooks *hooks);
void queue_menu_attempts_exhausted(const MenuOutcomeHooks *hooks);
#endif
