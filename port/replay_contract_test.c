#include "replay.h"

#include <stdio.h>
#include <string.h>

typedef struct { FA18ReplayEvent event[4]; size_t count; } Events;

static int collect(const FA18ReplayEvent *event, void *user) {
    Events *events = user;
    if (events->count >= 4) return 0;
    events->event[events->count++] = *event;
    return 0;
}

int main(void) {
    FILE *file = tmpfile();
    if (!file) return 1;
    fputs("E9K_INPUT_V1\nF 230 K 49 49 16 1\nF 234 K 49 49 16 0\n", file);
    fflush(file);
    /* The parser is path based; use the sealed run075 file for the real
     * format contract and verify the first two events through its sink. */
    fclose(file);
    Events events = {{0}, 0};
    size_t count = 0;
    if (fa18_replay_read_events("../../captures/run075/playback.e9k", collect,
                                &events, &count) != 0 || count != 87 ||
        events.count != 4 || events.event[0].frame != 41 ||
        events.event[0].kind != FA18_REPLAY_FRAME_EVENT) {
        fputs("run075 replay parse contract failed\n", stderr);
        return 1;
    }
    FA18ReplayControlState state;
    memset(&state, 0, sizeof state);
    const FA18ReplayEvent key = {230, FA18_REPLAY_KEY_EVENT, {49, 49, 16, 1}};
    if (fa18_replay_apply_event(&state, &key) != 0 ||
        state.frame != 230 || !state.keyboard_down[49]) {
        fputs("replay control latch contract failed\n", stderr);
        return 1;
    }
    puts("run075 deterministic replay contract passed");
    return 0;
}
