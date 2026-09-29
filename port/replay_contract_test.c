#include "replay.h"

#include <stdio.h>
#include <string.h>

typedef struct { FA18ReplayEvent event[128]; size_t count; } Events;

typedef struct { size_t count; int saw_j; } EventKinds;
typedef struct { FA18ReplayTickRange range[9]; size_t count; } TickRanges;

static int collect(const FA18ReplayEvent *event, void *user) {
    Events *events = user;
    if (events->count >= 128) return 0;
    events->event[events->count++] = *event;
    return 0;
}

static int collect_run060(const FA18ReplayEvent *event, void *user) {
    EventKinds *kinds = user;
    ++kinds->count;
    if (event->kind == FA18_REPLAY_JOYSTICK_EVENT && event->value[0] == 0 &&
        (event->value[1] == 5 || event->value[1] == 6 || event->value[1] == 7)) {
        kinds->saw_j = 1;
    }
    return 0;
}

static int collect_tick_range(const FA18ReplayTickRange *range, void *user) {
    TickRanges *ranges = user;
    if (ranges->count >= 9) return -1;
    ranges->range[ranges->count++] = *range;
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
    if (fa18_replay_read_events("../../captures/uae/run075/playback.e9k", collect,
                                &events, &count) != 0 || count != 87 ||
        events.count != 87 || events.event[0].frame != 41 ||
        events.event[0].kind != FA18_REPLAY_FRAME_EVENT) {
        fputs("run075 replay parse contract failed\n", stderr);
        return 1;
    }
    if (events.event[events.count - 1].frame != 21069 ||
        events.event[events.count - 1].value[1] != 7 ||
        events.event[events.count - 1].value[2] != -25) {
        fputs("signed replay parse contract failed\n", stderr);
        return 1;
    }
    TickRanges ticks = {{0}, 0};
    if (fa18_replay_read_tick_ranges("../../captures/uae/run075/timing.e9t",
                                     collect_tick_range, &ticks, NULL) != 0 ||
        ticks.count != 9 || ticks.range[0].first_frame != 235 ||
        ticks.range[1].last_frame != 240 || ticks.range[2].ticks != 6 ||
        ticks.range[3].first_frame != 271 || ticks.range[4].first_frame != 290 ||
        ticks.range[8].first_frame != 369 ||
        fa18_replay_ticks_for_frame(ticks.range, ticks.count, 234) != 0 ||
        fa18_replay_ticks_for_frame(ticks.range, ticks.count, 235) != 1 ||
        fa18_replay_ticks_for_frame(ticks.range, ticks.count, 240) != 5 ||
        fa18_replay_ticks_for_frame(ticks.range, ticks.count, 270) != 6 ||
        fa18_replay_ticks_for_frame(ticks.range, ticks.count, 271) != 5 ||
        fa18_replay_ticks_for_frame(ticks.range, ticks.count, 290) != 1 ||
        fa18_replay_ticks_for_frame(ticks.range, ticks.count, 291) != 0 ||
        fa18_replay_ticks_for_frame(ticks.range, ticks.count, 369) != 1 ||
        fa18_replay_ticks_for_frame(ticks.range, ticks.count, 272) != 0) {
        fputs("run075 replay timing contract failed\n", stderr);
        return 1;
    }
    EventKinds run060 = {0, 0};
    if (fa18_replay_read_events("../../captures/uae/run060/playback.e9k",
                                collect_run060, &run060, NULL) != 0 ||
        run060.count == 0 || !run060.saw_j) {
        fputs("run060 joystick replay parse contract failed\n", stderr);
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
    const FA18ReplayEvent joystick = {939, FA18_REPLAY_JOYSTICK_EVENT,
                                      {0, 5, 1, 0}};
    if (fa18_replay_apply_event(&state, &joystick) != 0 ||
        !state.joystick[0][5] || state.packed_flight_control != 0x20) {
        fputs("joystick control latch contract failed\n", stderr);
        return 1;
    }
    FA18ReplayEvent events_for_frame[] = {
        {230, FA18_REPLAY_KEY_EVENT, {49, 49, 16, 1}},
        {234, FA18_REPLAY_KEY_EVENT, {49, 49, 16, 0}}
    };
    memset(&state, 0, sizeof state);
    size_t next = 0;
    if (fa18_replay_advance_frame(&state, events_for_frame, 2, &next, 230) != 0 ||
        next != 1 || state.frame != 230 || !state.keyboard_down[49] ||
        fa18_replay_advance_frame(&state, events_for_frame, 2, &next, 233) != 0 ||
        next != 1 || !state.keyboard_down[49] ||
        fa18_replay_advance_frame(&state, events_for_frame, 2, &next, 234) != 0 ||
        next != 2 || state.keyboard_down[49]) {
        fputs("replay frame advance contract failed\n", stderr);
        return 1;
    }
    FA18ReplayEvent motion_events[] = {
        {10, FA18_REPLAY_FRAME_EVENT, {4, -12, 3, 0}},
        {10, FA18_REPLAY_FRAME_EVENT, {4, 2, -1, 0}}
    };
    memset(&state, 0, sizeof state);
    next = 0;
    if (fa18_replay_advance_frame(&state, motion_events, 2, &next, 10) != 0 ||
        state.motion[0][0] != -10 || state.motion[3][1] != 2 ||
        fa18_replay_advance_frame(&state, motion_events, 2, &next, 11) != 0 ||
        state.motion[0][0] != 0 || state.motion[3][1] != 0) {
        fputs("per-frame motion lifetime contract failed\n", stderr);
        return 1;
    }
    puts("run075 deterministic replay contract passed");
    return 0;
}
