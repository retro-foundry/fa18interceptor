#ifndef FA18_NATIVE_COMMAND_DISPATCH_H
#define FA18_NATIVE_COMMAND_DISPATCH_H

#include "command_queue.h"
#include "input_callback_registration.h"

typedef struct {
    FA18ContextCommandState *context;
    FA18CommandQueue *queue;
    FA18InputCallbackRegistration *input_registration;
    uint16_t error_code;
} FA18NativeCommandDispatcher;

typedef struct {
    const FA18IndexedControlPoses *indexed_poses;
    const FA18ViewSpanOffsets *spans;
    const FA18ContextCommandPoses *context_poses;
    FA18IndexedStatusTone status_tone;
    void *status_context;
    const FA18FlightCommandOps *flight;
    const FA18ContextCommandOps *context;
} FA18NativeCommandOwners;

typedef enum {
    FA18_COMMAND_UNPUBLISHED, FA18_COMMAND_PUBLISHED,
    FA18_COMMAND_INVALID_PENDING, FA18_COMMAND_INPUT_RESET
} FA18NativeCommandCompletion;
typedef struct {
    enum CommandAction action;
    FA18NativeCommandCompletion completion;
    uint32_t event; /* meaningful only when PUBLISHED */
} FA18NativeCommandOutcome;

/* Real direction/throttle/space-release children and the complete $C1C214
 * eject toggle, including its nested publication. Returns 0 for other child
 * families or missing state. command_effects.c supplies the other game
 * children with original data and a required host audio acknowledgement. */
int fa18_is_native_flight_child(enum FlightCommandChild child);
int fa18_apply_native_flight_child(FA18NativeCommandDispatcher *state,
                                   enum FlightCommandChild child,
                                   const FA18FlightCommandChildInput *input,
                                   FlightCommandResult *result);

/* Complete $C1AD74/$C1AC28 parents, composed from the actual native owners.
 * The caller supplies the inherited action word; keyboard selection updates
 * it before action execution. Reset executes both registration children;
 * the original $C06C02 release-build fault hook has no game-state effects.
 * Returns 0 on missing data/child failure; preceding source writes remain.
 * Outcome is assigned only on success. Neither entry runs an interpreter. */
int fa18_dispatch_native_keyboard_command(FA18NativeCommandDispatcher *state,
                                           uint32_t event, int16_t carried_word,
                                           const FA18NativeCommandOwners *owners,
                                           FA18NativeCommandOutcome *outcome);
int fa18_dispatch_native_pending_command(FA18NativeCommandDispatcher *state,
                                          int16_t carried_word,
                                          const FA18NativeCommandOwners *owners,
                                          FA18NativeCommandOutcome *outcome);

#endif
