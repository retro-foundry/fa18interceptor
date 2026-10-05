#ifndef FA18_CONTEXT_COMMAND_INPUT_H
#define FA18_CONTEXT_COMMAND_INPUT_H

#include "view_command_input.h"
#include "context_command_types.h"
#include <stddef.h>

/* Context-owned aircraft values. Identity is the same ordinary record pointer
 * used by flight/view commands; geometry preserves original 32-bit wrapping. */
typedef struct {
    FA18FlightCommandRecord *command_record;
    uint16_t angle;
    uint32_t position[3];
    int16_t inverse[3][3];
} FA18ContextCommandRecord;

/* Resolve negative pose words to the actual record when importing data,
 * using the original wrapped signed-word displacement. For preset poses,
 * import six original words and the grid pair selected by word 2 * 4 with
 * signed-word wrap. No synthetic coordinates or grid values are supplied. */
typedef enum {
    FA18_CONTEXT_POSE_UNRESOLVED, FA18_CONTEXT_POSE_PRESET, FA18_CONTEXT_POSE_RECORD
} FA18ContextCommandPoseKind;
typedef struct {
    FA18ContextCommandPoseKind kind;
    FA18ContextCommandRecord *record;
    int16_t preset[6], grid_pair[2];
} FA18ContextCommandPose;
typedef struct {
    const FA18ContextCommandPose *values;
    size_t count;
    int first_index; /* signed scene-pose byte of values[0] */
} FA18ContextCommandPoses;

typedef struct {
    FA18ViewCommandState *view;
    FA18ContextCommandRecord *records;
    size_t record_count;
    uint8_t *key_taken; /* queue-owned byte, shared with publication */
    uint32_t origin_first, origin_third, map_middle_cache, negated[3];
    uint32_t smoothed_delta, auxiliary_delta[2];
    uint16_t angle_history, pan, rotate;
    uint8_t view_request, track_started, recorder_on;
    /* NULL means the source has no positive recorder cursor. The recorder
     * owner resolves a valid destination; remaining bytes must include 4. */
    uint8_t *recording_write;
    size_t recording_remaining;
} FA18ContextCommandState;

typedef struct {
    const FA18ContextCommandRecord *record;
    int16_t local[3];
    int32_t position[3];
    uint32_t event;
} FA18ContextCommandChildInput;
typedef struct { uint32_t event; int32_t position[3]; } FA18ContextCommandChildResult;
typedef struct {
    /* Only the two $C0F4A6 voice-release calls reach this owner. It may change
     * shared state before the parent resumes; return 0 for missing/failing
     * implementation. Geometry/observer children use actual native code. */
    int (*consume)(void *context, FA18ContextCommandState *state,
                   enum ContextCommandChild child,
                   const FA18ContextCommandChildInput *input,
                   FA18ContextCommandChildResult *result);
    void *context;
} FA18ContextCommandOps;

int fa18_is_context_input_command(enum CommandAction action);
int fa18_apply_context_control_child(FA18ContextCommandState *state,
                                     enum ContextCommandChild child,
                                     const FA18ContextCommandChildInput *input,
                                     FA18ContextCommandChildResult *result);

/* All five context actions of $C1AC28/$C1AD74 before queue publication.
 * Pose data (including explicit preset/record kind) and viewed record identity must be resolved by their native
 * owners. Return 0 for invalid arguments, missing data or child failure.
 * Source writes can precede a data/child error; event is assigned on success. */
int fa18_apply_context_input_command(FA18ContextCommandState *state,
                                     const CommandRequest *request,
                                     const FA18ContextCommandPoses *poses,
                                     const FA18ContextCommandOps *ops,
                                     uint32_t *published_event);

#endif
