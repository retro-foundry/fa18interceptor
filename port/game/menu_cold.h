#ifndef FA18_MENU_COLD_H
#define FA18_MENU_COLD_H
#include "memory.h"
enum MenuColdChild { MENU_COLD_LOAD, MENU_COLD_CLEAR_TABLE, MENU_COLD_RESET,
    MENU_COLD_CLEAR_SUMMARY, MENU_COLD_SUMMARY, MENU_COLD_CLEAR_QUEUE,
    MENU_COLD_POSITION, MENU_COLD_UPDATE };
enum MenuColdPhase { MC_BYTE_TEST, MC_BYTE_D0_SUBTRACT, MC_WORD_D0,
    MC_WORD_COMPARE, MC_BYTE_STORE, MC_WORD_STORE, MC_CALLBACK,
    MC_QUEUE_START, MC_QUEUE_LOCAL, MC_QUEUE_MODE, MC_QUEUE_ENABLED,
    MC_QUEUE_CODE, MC_QUEUE_NEXT, MC_QUEUE_ADVANCE, MC_QUEUE_END,
    MC_TABLE_START, MC_TABLE_WORD, MC_TABLE_NEXT, MC_TABLE_END,
    MC_COCKPIT, MC_POSITION_PRESET };
typedef struct {
    void (*consume)(void *context,enum MenuColdChild child);
    void (*observe)(void *context,enum MenuColdPhase phase,uint32_t value,gaddr address);
    void *context;
} MenuColdHooks;
void consume_menu_table_action(const MenuColdHooks *hooks);
void queue_available_menu_modes(const MenuColdHooks *hooks);
void leave_menu_after_countdown(const MenuColdHooks *hooks,int set_context);
void set_menu_position_preset(const MenuColdHooks *hooks,int alternate);
void refresh_menu_cockpit(const MenuColdHooks *hooks);
void clear_menu_mode_table(const MenuColdHooks *hooks);
#endif
