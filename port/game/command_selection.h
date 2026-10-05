#ifndef FA18_COMMAND_SELECTION_H
#define FA18_COMMAND_SELECTION_H
#include "memory.h"
#include "command_types.h"

/* Dispatch policy component of the complete C1AC28/C1AD74 owners.
 * Selecting an action is separate from executing its shared command tail.
 * This component alone is not a completed/registered original routine. */
enum CommandSelectionPhase {
    COMMAND_READ_EVENT, COMMAND_TEST_COUNTER, COMMAND_INCREMENT_COUNTER,
    COMMAND_CLEAR_RELEASE, COMMAND_READ_ORIGIN, COMMAND_READ_MODIFIER,
    COMMAND_READ_DETAIL, COMMAND_COMPARE_DETAIL, COMMAND_READ_RECORDER,
    COMMAND_COMPARE_RECORDER, COMMAND_COMPARE_MODE, COMMAND_COMPARE_GATE,
    COMMAND_TEST_BYTE, COMMAND_COMPARE_KEY, COMMAND_READ_BLOCK,
    COMMAND_SET_INDEX, COMMAND_SET_LATCH, COMMAND_CLEAR_CONTEXT,
    COMMAND_READ_WORD_A, COMMAND_MASK_WORD_A, COMMAND_PENDING_BEGIN,
    COMMAND_SELECT_WORD_B, COMMAND_TEST_WORD, COMMAND_CLEAR_WORD_BIT,
    COMMAND_CLEAR_LATCH
};
typedef struct {
    void (*observe)(void *context, enum CommandSelectionPhase phase,
                    uint32_t value, uint32_t limit);
    void *context;
} CommandSelectionHooks;
CommandRequest select_keyboard_command(uint32_t raw_event,
                                       const CommandSelectionHooks *hooks);
CommandRequest select_pending_command(const CommandSelectionHooks *hooks);
#endif
