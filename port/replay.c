#include "replay.h"

#include <stdio.h>
#include <string.h>

int fa18_replay_read_events(const char *path, FA18ReplayEventSink sink, void *user,
                            size_t *event_count) {
    if (!path || !sink) return -1;
    FILE *file = fopen(path, "rb");
    if (!file) return -1;
    char line[256];
    size_t count = 0;
    int valid_header = 0;
    while (fgets(line, sizeof line, file)) {
        if (!valid_header && strcmp(line, "E9K_INPUT_V1\n") != 0 &&
            strcmp(line, "E9K_INPUT_V1\r\n") != 0) {
            fclose(file);
            return -1;
        }
        if (!valid_header) {
            valid_header = 1;
            continue;
        }
        FA18ReplayEvent event;
        memset(&event, 0, sizeof event);
        unsigned frame, a, b, c, d = 0;
        char kind;
        int fields = sscanf(line, "F %u %c %u %u %u %u", &frame, &kind,
                            &a, &b, &c, &d);
        if (fields < 5) continue;
        event.frame = frame;
        if (kind == 'm') {
            event.kind = FA18_REPLAY_FRAME_EVENT;
            if (fields != 5) {
                fclose(file);
                return -1;
            }
        } else if (kind == 'K') {
            event.kind = FA18_REPLAY_KEY_EVENT;
            if (fields != 6) {
                fclose(file);
                return -1;
            }
        } else {
            fclose(file);
            return -1;
        }
        event.value[0] = (int32_t)a;
        event.value[1] = (int32_t)b;
        event.value[2] = (int32_t)c;
        event.value[3] = (int32_t)d;
        if (sink(&event, user) != 0) {
            fclose(file);
            return -1;
        }
        ++count;
    }
    fclose(file);
    if (!valid_header) return -1;
    if (event_count) *event_count = count;
    return 0;
}

int fa18_replay_apply_event(FA18ReplayControlState *state,
                            const FA18ReplayEvent *event) {
    if (!state || !event || event->frame < state->frame) return -1;
    state->frame = event->frame;
    if (event->kind == FA18_REPLAY_FRAME_EVENT) {
        for (size_t index = 0; index < 4; ++index) {
            state->joystick[index] = (uint8_t)(event->value[index] & 0xff);
        }
        return 0;
    }
    if (event->kind == FA18_REPLAY_KEY_EVENT && event->value[0] >= 0 &&
        event->value[0] < 256) {
        state->keyboard_down[event->value[0]] = (uint8_t)(event->value[3] != 0);
        return 0;
    }
    return -1;
}
