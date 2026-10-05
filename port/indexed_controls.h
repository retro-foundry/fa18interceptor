#ifndef FA18_INDEXED_CONTROLS_H
#define FA18_INDEXED_CONTROLS_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    FA18_INDEXED_FUNCTION_KEY,
    FA18_INDEXED_LOW_KEY,
    FA18_INDEXED_SELECTION
} FA18IndexedControlKind;

typedef struct {
    FA18IndexedControlKind kind;
    uint32_t event;
    uint8_t modifier;
    int16_t selection;
} FA18IndexedControlRequest;

/* Named values from the original mode record: word +0, byte +6 and
 * availability bytes +$12..+$19. Import these from the original save/data
 * record; zero is meaningful and must not be replaced with default unlocks. */
typedef struct {
    uint16_t status;
    uint8_t level;
    uint8_t available[8];
} FA18IndexedControlModes;

typedef struct {
    uint8_t mode, mode_request, mode_gate;
    uint8_t enable_gate, enable_selection;
    uint8_t function_modifier, recorder_mode;
    uint8_t cockpit_high_byte, pose_entry, pose_inhibit;
    uint8_t origin_detail, origin_gate_a, origin_gate_b;
    uint8_t function_level, control_record_level, player_ready;
    uint32_t playback_bytes;
    int16_t throttle, throttle_companion;
    FA18IndexedControlModes modes;
} FA18IndexedControls;

/* First signed word of each original 16-byte SCENE_POSE_TABLE record, in
 * source order. The remaining pose data belongs to the scene subsystem. */
typedef struct {
    const int16_t *values;
    size_t count;
} FA18IndexedControlPoses;

/* Original $C3318E status-tone child (tone 2, pitch 2 while volume fades,
 * otherwise pitch 4). Call it after storing the selected mode; the event
 * to publish comes from the child result. Audio state belongs to context.
 * The oracle can also check child inputs and controlled state effects. */
typedef uint32_t (*FA18IndexedStatusTone)(void *context,
                                        FA18IndexedControls *state);

/* Complete indexed-action component of $C1AC28/$C1AD74, $C1BC50-$C1BEE4
 * and shared $C1C214 toggle. Selection and queue publication are separate.
 * No CPU registers, guest addresses, memory bus, ROM or instruction handlers.
 * Returns 0 for invalid arguments or an exhausted pose table. An incomplete
 * table is an asset error, not an implicit pose terminator.
 * The carried selection word is an input to the rejected function-key path.
 * `published_event` is assigned only on success. */
int fa18_apply_indexed_control(FA18IndexedControls *state,
                              const FA18IndexedControlRequest *request,
                              int16_t carried_selection,
                              const FA18IndexedControlPoses *poses,
                              FA18IndexedStatusTone status_tone,
                              void *context, uint32_t *published_event);

#endif
