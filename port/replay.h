#ifndef FA18_REPLAY_H
#define FA18_REPLAY_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    FA18_REPLAY_FRAME_EVENT = 1,
    FA18_REPLAY_KEY_EVENT = 2
} FA18ReplayEventKind;

typedef struct {
    uint32_t frame;
    FA18ReplayEventKind kind;
    int32_t value[4];
} FA18ReplayEvent;

typedef struct {
    uint32_t frame;
    uint8_t joystick[4];
    uint8_t keyboard_down[256];
} FA18ReplayControlState;

typedef int (*FA18ReplayEventSink)(const FA18ReplayEvent *event, void *user);

/* Read the deterministic E9K_INPUT_V1 text stream used by Engine9000. */
int fa18_replay_read_events(const char *path, FA18ReplayEventSink sink, void *user,
                            size_t *event_count);

/* Apply one event to the native control latch. The cockpit update will consume
 * this latch when that frame gate is reached. */
int fa18_replay_apply_event(FA18ReplayControlState *state,
                            const FA18ReplayEvent *event);

#endif
