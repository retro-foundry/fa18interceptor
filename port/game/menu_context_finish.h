#ifndef FA18_MENU_CONTEXT_FINISH_H
#define FA18_MENU_CONTEXT_FINISH_H
#include "memory.h"

enum MenuContextChild {
    MC_CANCEL, MC_SMOOTH_RESET, MC_RESTART_RESET, MC_REFRESH_VIEW,
    MC_PRESET_POSITION, MC_NOISE, MC_ENGINE, MC_HEADING,
    MC_MESSAGE_TIME, MC_EXPIRY_TONE, MC_STAGE_SETUP, MC_STAGE_SOUND,
    MC_STAGE_COMMAND, MC_STAGE_TIME, MC_STAGE_RESET, MC_STAGE_FINISH,
    MC_VIEWPORT_RESET, MC_VIEWPORT_TIME, MC_POSITION_TRANSFORM, MC_TIMER_REQUEST
};
enum MenuContextPhase {
    MC_BYTE_STORE, MC_WORD_STORE, MC_LONG_STORE, MC_BYTE_TEST, MC_WORD_TEST,
    MC_LONG_TEST, MC_D0_BYTE, MC_D0_WORD, MC_D0_LONG, MC_D1_LONG,
    MC_SUB_BYTE, MC_CMP_BYTE, MC_CMP_LONG, MC_BIT_BYTE, MC_BIT_WORD,
    MC_CALLBACK, MC_VIEWPORT_COMPARE, MC_WORD_OR_D0, MC_WORD_AND_D0,
    MC_ADD_WORD_D0, MC_SHIFT_WORD_D0, MC_LOCAL_CURSOR, MC_LOCAL_BYTE,
    MC_QUEUE_WORD, MC_QUEUE_NEXT, MC_SUB_LONG_D1, MC_ADD_MEMORY_D1,
    MC_ADD_MEMORY_D0, MC_PRESET_INPUT, MC_TIMER_ZERO, MC_TABLE_POINTER
};
typedef struct {
    int32_t (*consume)(void *context,enum MenuContextChild child);
    void (*observe)(void *context,enum MenuContextPhase phase,uint32_t value,gaddr address);
    void (*position_result)(void *context,uint32_t result[3]);
    void *context;
} MenuContextHooks;
void follow_menu_smoothing(const MenuContextHooks *hooks);
void queue_menu_smoothing_message(const MenuContextHooks *hooks);
void restart_menu_smoothing(const MenuContextHooks *hooks);
void reset_menu_smoothing_view(const MenuContextHooks *hooks);
void begin_menu_context(const MenuContextHooks *hooks);
void queue_menu_context_command(gaddr frame,const MenuContextHooks *hooks);
void finish_menu_context_message(const MenuContextHooks *hooks);
void expire_menu_context(const MenuContextHooks *hooks);
void update_menu_context(gaddr frame,const MenuContextHooks *hooks);
void queue_menu_viewport_message(const MenuContextHooks *hooks);
void finish_menu_viewport_message(const MenuContextHooks *hooks);
void load_menu_position_preset(const MenuContextHooks *hooks);
void read_menu_time_sample(const MenuContextHooks *hooks);
#endif
