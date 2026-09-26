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
        unsigned frame;
        int a, b, c, d = 0;
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
        } else if (kind == 'J') {
            event.kind = FA18_REPLAY_JOYSTICK_EVENT;
            if (fields != 5) {
                fclose(file);
                return -1;
            }
        } else {
            fclose(file);
            return -1;
        }
        event.value[0] = a;
        event.value[1] = b;
        event.value[2] = c;
        event.value[3] = d;
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
        if (event->value[0] < 0 || event->value[0] > 4) return -1;
        int first_port = event->value[0] == 4 ? 0 : event->value[0];
        int last_port = event->value[0] == 4 ? 4 : event->value[0] + 1;
        for (int port = first_port; port < last_port; ++port) {
            state->motion[port][0] += event->value[1];
            state->motion[port][1] += event->value[2];
        }
        return 0;
    }
    if (event->kind == FA18_REPLAY_KEY_EVENT && event->value[0] >= 0 &&
        event->value[0] < 256) {
        state->keyboard_down[event->value[0]] = (uint8_t)(event->value[3] != 0);
        return 0;
    }
    if (event->kind == FA18_REPLAY_JOYSTICK_EVENT &&
        event->value[0] >= 0 && event->value[0] < 4 &&
        event->value[1] >= 0 && event->value[1] < 8) {
        state->joystick[event->value[0]][event->value[1]] =
            (uint8_t)(event->value[2] != 0);
        if (event->value[0] == 0) {
            uint8_t field_mask = 0;
            uint8_t command = 0;
            switch (event->value[1]) {
            case 4: field_mask = 0x30; command = 0x10; break;
            case 5: field_mask = 0x30; command = 0x20; break;
            case 6: field_mask = 0x0c; command = 0x08; break;
            case 7: field_mask = 0x0c; command = 0x04; break;
            default: break;
            }
            if (field_mask) {
                state->packed_flight_control = (uint8_t)(
                    (state->packed_flight_control & (uint8_t)~field_mask) |
                    (event->value[2] ? command : 0));
            }
        }
        return 0;
    }
    return -1;
}

int fa18_replay_advance_frame(FA18ReplayControlState *state,
                              const FA18ReplayEvent *events, size_t event_count,
                              size_t *next_event, uint32_t frame) {
    if (!state || (!events && event_count != 0) || !next_event ||
        *next_event > event_count) return -1;
    /* Engine9000 publishes relative motion during input_poll and clears the
     * pending sample immediately afterward. Preserve that per-frame lifetime
     * while allowing all events at this frame to add to the same sample. */
    memset(state->motion, 0, sizeof state->motion);
    while (*next_event < event_count && events[*next_event].frame <= frame) {
        if (fa18_replay_apply_event(state, &events[*next_event]) != 0) return -1;
        ++*next_event;
    }
    state->frame = frame;
    return 0;
}
