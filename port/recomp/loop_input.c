/* Native recordings keyed to the game's main loop. */
#include "loop_input.h"

#include <stdlib.h>
#include <string.h>

#include "machine.h"

typedef struct {
    long iteration;
    char kind;
    int a, b, c, d;
} LoopEvent;

static FILE *record_out;
static LoopEvent *events;
static int event_count, next_event;
static long iteration, frame, recorded_end;

/* Live input waiting for the next iteration. */
static struct {
    int keys[64][2];
    int key_count;
    int dx, dy;
    int buttons[3], sent_buttons[3];
} held = {{{0}}, 0, 0, 0, {0, 0, 0}, {0, 0, 0}};

void fa18_loop_record(FILE *out) {
    record_out = out;
    fprintf(out, "FA18_LOOP_INPUT_V1\n");
}

int fa18_loop_replay(const char *path) {
    FILE *f = fopen(path, "r");
    char line[256];
    int capacity = 0;
    if (!f) return 0;
    if (!fgets(line, sizeof line, f) || strncmp(line, "FA18_LOOP_INPUT_V1", 18)) { fclose(f); return 0; }
    while (fgets(line, sizeof line, f)) {
        LoopEvent e = {0, 0, 0, 0, 0, 0};
        long at_frame;
        if (!strncmp(line, "end", 3)) {
            sscanf(line, "end %ld", &recorded_end);
            continue;
        }
        if (line[0] == '#') continue;
        if (sscanf(line, "%ld %ld %c %d %d %d %d", &e.iteration, &at_frame, &e.kind, &e.a, &e.b, &e.c, &e.d) < 4)
            continue;
        if (event_count == capacity) {
            capacity = capacity ? capacity * 2 : 256;
            events = realloc(events, (size_t)capacity * sizeof *events);
        }
        events[event_count++] = e;
    }
    fclose(f);
    return 1;
}

void fa18_loop_finish(void) {
    if (!record_out) return;
    fprintf(record_out, "end %ld %ld\n", iteration, frame);
    fclose(record_out);
    record_out = NULL;
}

void fa18_loop_host_key(int rawkey, int down) {
    if (held.key_count < 64) {
        held.keys[held.key_count][0] = rawkey;
        held.keys[held.key_count][1] = down;
        held.key_count++;
    }
}

void fa18_loop_host_mouse(int dx, int dy) {
    held.dx += dx;
    held.dy += dy;
}

void fa18_loop_host_button(int button, int down) {
    if (button >= 0 && button < 3) held.buttons[button] = down;
}

static int clamp127(int v) { return v > 127 ? 127 : v < -127 ? -127 : v; }

/* The mouse counters move by the whole delta now (JOY0DAT follows). */
static void move_mouse(FA18Machine *m, int dx, int dy) {
    m->mouse_x = (m->mouse_x + dx) & 0xFF;
    m->mouse_y = (m->mouse_y + dy) & 0xFF;
    m->joy0dat = (uint16_t)(m->mouse_y << 8 | m->mouse_x);
}

static void deliver(FA18Machine *m, const LoopEvent *e) {
    switch (e->kind) {
    case 'K': fa18_machine_key(m, e->a, e->b); break;
    case 'M': move_mouse(m, e->a, e->b); break;
    case 'B': fa18_machine_button(m, e->a, e->b); break;
    case 'J': fa18_machine_joystick(m, e->a, e->b, e->c, e->d); break;
    default: break;
    }
}

static void emit(FA18Machine *m, char kind, int a, int b, int c, int d) {
    LoopEvent e;
    e.iteration = iteration;
    e.kind = kind;
    e.a = a; e.b = b; e.c = c; e.d = d;
    deliver(m, &e);
    if (kind == 'J') fprintf(record_out, "%ld %ld J %d %d %d %d\n", iteration, frame, a, b, c, d);
    else fprintf(record_out, "%ld %ld %c %d %d\n", iteration, frame, kind, a, b);
}

/* FA18_LOOP_DUMP=N:path writes Chip and Slow RAM as iteration N starts
 * (before its input), for comparing runs at the same point in the game. */
static void dump_if_asked(FA18Machine *m) {
    static long at = -1;
    static const char *path;
    if (at == -1) {
        const char *v = getenv("FA18_LOOP_DUMP");
        at = -2;
        if (v && strchr(v, ':')) { at = atol(v); path = strchr(v, ':') + 1; }
    }
    if (at == iteration && path) {
        FILE *f = fopen(path, "wb");
        if (f) {
            fwrite(m->chip, 1, FA18_CHIP_SIZE, f);
            fwrite(m->slow, 1, FA18_SLOW_SIZE, f);
            fclose(f);
        }
    }
}

void fa18_loop_iteration(void) {
    FA18Machine *m = fa18_machine;
    iteration++;
    dump_if_asked(m);
    if (record_out) {
        int i, dx = clamp127(held.dx), dy = clamp127(held.dy);
        for (i = 0; i < held.key_count; i++) emit(m, 'K', held.keys[i][0], held.keys[i][1], 0, 0);
        held.key_count = 0;
        for (i = 0; i < 3; i++)
            if (held.buttons[i] != held.sent_buttons[i]) {
                emit(m, 'B', i, held.buttons[i], 0, 0);
                held.sent_buttons[i] = held.buttons[i];
            }
        if (dx || dy) {
            emit(m, 'M', dx, dy, 0, 0);
            held.dx -= dx;
            held.dy -= dy;
        }
        return;
    }
    while (next_event < event_count && events[next_event].iteration <= iteration) {
        if (events[next_event].iteration == iteration) deliver(m, &events[next_event]);
        next_event++;
    }
}

void fa18_loop_frame(void) { frame++; }

int fa18_loop_recording(void) { return record_out != NULL; }
long fa18_loop_iterations(void) { return iteration; }
long fa18_loop_replay_end(void) {
    if (recorded_end) return recorded_end;
    return event_count ? events[event_count - 1].iteration + 1 : 0;
}
