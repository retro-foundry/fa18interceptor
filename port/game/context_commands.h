#ifndef FA18_CONTEXT_COMMANDS_H
#define FA18_CONTEXT_COMMANDS_H
#include "command_selection.h"
enum ContextCommandChild {
    CONTEXT_COMMAND_LOCAL_TO_WORLD, CONTEXT_COMMAND_SET_OBSERVER,
    CONTEXT_COMMAND_MAP_VOICES, CONTEXT_COMMAND_REQUEST_VOICES
};
enum ContextCommandPhase {
    CONTEXT_BYTE_TEST, CONTEXT_WORD_STORE, CONTEXT_BYTE_STORE, CONTEXT_LONG_STORE,
    CONTEXT_REQUEST_BIT, CONTEXT_MODIFIER_TEST, CONTEXT_ORIGIN_TEST,
    CONTEXT_RECORD_COPY_BEGIN, CONTEXT_RECORD_COPY_OFFSET,
    CONTEXT_POSE_BEGIN, CONTEXT_POSE_INDEX, CONTEXT_POSE_VALUE,
    CONTEXT_RECORD_SELECT, CONTEXT_RECORD_COMPARE, CONTEXT_LOCAL_POINT,
    CONTEXT_PRESET_POSITION, CONTEXT_EVENT_SAVE, CONTEXT_EVENT_RESTORE,
    CONTEXT_MAP_POSITION, CONTEXT_MAP_NEGATE, CONTEXT_MAP_CACHED_POSITION,
    CONTEXT_GATE_ADDRESS, CONTEXT_RECORDER_CURSOR, CONTEXT_RECORDER_CLEAR_BEGIN,
    CONTEXT_RECORDER_CLEAR_BYTE
};
typedef struct { gaddr record; int16_t local[3]; int32_t position[3]; } ContextCommandInput;
typedef struct { uint32_t event; int32_t position[3]; } ContextCommandResult;
typedef struct {
    ContextCommandResult (*consume)(void *context,enum ContextCommandChild child,
                                   const ContextCommandInput *input);
    void (*observe)(void *context,enum ContextCommandPhase phase,
                    uint32_t value,uint32_t limit,gaddr address);
    void *context;
} ContextCommandHooks;
int is_context_command(enum CommandAction action);
uint32_t execute_context_command(const CommandRequest *request,const ContextCommandHooks *hooks);
#endif
