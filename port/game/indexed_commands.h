#ifndef FA18_INDEXED_COMMANDS_H
#define FA18_INDEXED_COMMANDS_H
#include "command_selection.h"
enum IndexedCommandPhase {
    INDEXED_BYTE_TEST, INDEXED_WORD_TEST, INDEXED_LONG_TEST,
    INDEXED_BYTE_STORE, INDEXED_WORD_STORE, INDEXED_MODIFIER_TEST,
    INDEXED_EVENT_COMPARE, INDEXED_EVENT_COPY, INDEXED_SUBTRACT_WORD,
    INDEXED_ADD_WORD, INDEXED_COMPARE_WORD, INDEXED_COMPARE_BYTE,
    INDEXED_SELECT_BYTE, INDEXED_SELECT_CONSTANT,
    INDEXED_RECORDER_READ, INDEXED_RECORDER_COMPARE, INDEXED_RECORDER_TEST,
    INDEXED_LEVEL_DOUBLE, INDEXED_LEVEL_SAVE, INDEXED_LEVEL_ADD,
    INDEXED_LEVEL_INCREMENT, INDEXED_LEVEL_EXTEND, INDEXED_LEVEL_SCALE,
    INDEXED_LEVEL_COMPARE, INDEXED_LEVEL_CLAMP,
    INDEXED_MODE_TABLE, INDEXED_TABLE_LEVEL_READ, INDEXED_TABLE_LEVEL_INCREMENT,
    INDEXED_TABLE_AVAILABILITY, INDEXED_POSE_TEST,
    INDEXED_POSE_BEGIN, INDEXED_POSE_VALUE, INDEXED_POSE_COMPARE,
    INDEXED_POSE_ADVANCE, INDEXED_POSE_TERMINATOR, INDEXED_GATE_ADDRESS
};
typedef struct {
    uint32_t (*mode_changed)(void *context);
    void (*observe)(void *context,enum IndexedCommandPhase phase,
                    uint32_t value,uint32_t limit,gaddr address);
    void *context;
} IndexedCommandHooks;
int is_indexed_command(enum CommandAction action);
/* The rejected function-key path at C1BCEE uses the carried selection word
 * before assigning a new one. It is an explicit owner input. */
uint32_t execute_indexed_command(const CommandRequest *request,int16_t carried_index,
                                const IndexedCommandHooks *hooks);
#endif
