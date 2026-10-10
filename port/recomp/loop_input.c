/* Native recordings keyed to the game's main loop. */
#include "loop_input.h"

#include <stdlib.h>
#include <string.h>

#include "machine.h"
#include "flight_trace.h"

typedef struct {
    long iteration;
    char kind;
    int a, b, c, d;
} LoopEvent;

static FILE *record_out;
static FILE *game_out;
static LoopEvent *events;
static int event_count, next_event;
static long iteration, frame, recorded_end;
static FA18FlightTrace flight_trace;

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

void fa18_loop_game_record(FILE *out) {
    game_out = out;
    fputs("FA18_GAME_INPUT_V1\n", out);
}
int fa18_loop_game_recording(void) { return game_out != NULL; }

void fa18_loop_game_key(unsigned raw) {
    if (game_out)
        fprintf(game_out, "%ld %ld K %u %u\n", iteration, frame,
                raw & 0x7f, (raw & 0x80) ? 0u : 1u);
}

int fa18_loop_finish(void) {
    int ok = fa18_flight_trace_close(&flight_trace);
    FILE **outputs[] = {&record_out, &game_out};
    for (unsigned i = 0; i < sizeof outputs / sizeof outputs[0]; ++i) {
        FILE *out = *outputs[i];
        if (!out) continue;
        fprintf(out, "end %ld %ld\n", iteration, frame);
        if (ferror(out)) ok = 0;
        if (fclose(out)) ok = 0;
        *outputs[i] = NULL;
    }
    return ok;
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

/* FA18_LOOP_DUMP=N:path writes RAM as iteration N starts, before input.
 * N+COUNT:prefix retains consecutive boundaries as prefix.ITERATION.dat.
 * Diagnostic output never seeds game state or changes machine scheduling. */
static void dump_if_asked(FA18Machine *m) {
    static long at = -1;
    static long count = 1;
    static const char *path;
    if (at == -1) {
        const char *v = getenv("FA18_LOOP_DUMP");
        at = -2;
        if (v) {
            char *end;
            long first=strtol(v,&end,10);
            if (*end=='+') count=strtol(end+1,&end,10);
            if (*end!=':' || first<=0 || count<=0 || first>10000000 || count>10000000-first+1) {
                fputs("Invalid FA18_LOOP_DUMP (FIRST[+COUNT]:PATH)\n",stderr);exit(2);
            }
            at=first;path=end+1;
        }
    }
    if (path && iteration>=at && iteration-at<count) {
        char numbered[4096];
        const char *target=path;
        if (count!=1) {
            int length=snprintf(numbered,sizeof numbered,"%s.%ld.dat",path,iteration);
            if (length<0 || length>=(int)sizeof numbered) {
                fputs("FA18_LOOP_DUMP path is too long\n",stderr);exit(2);
            }
            target=numbered;
        }
        FILE *f = fopen(target, "wb");
        if (!f) {perror(target);exit(2);}
        int written=fwrite(m->chip,1,FA18_CHIP_SIZE,f)==FA18_CHIP_SIZE &&
                    fwrite(m->slow,1,FA18_SLOW_SIZE,f)==FA18_SLOW_SIZE;
        if (fclose(f) || !written) {
            fprintf(stderr,"Cannot write FA18_LOOP_DUMP: %s\n",target);exit(2);
        }
    }
}

static const uint8_t *trace_bytes(void *context,uint32_t address,size_t size) {
    const FA18Machine *machine=context;
    if(address<FA18_CHIP_SIZE && size<=FA18_CHIP_SIZE-address) return machine->chip+address;
    if(address>=0xc00000u && address<0xc00000u+FA18_SLOW_SIZE &&
       size<=0xc00000u+FA18_SLOW_SIZE-address) return machine->slow+address-0xc00000u;
    return NULL;
}
static void trace_if_asked(FA18Machine *machine) {
    static int initialized;
    if(!initialized) {
        initialized=1;
        const char *path=getenv("FA18_LOOP_TRACE");
        if(path) {
            size_t budget=512u*1024u*1024u;
            const char *value=getenv("FA18_LOOP_TRACE_BUDGET_MIB");
            if(value) {
                char *end;unsigned long mib=strtoul(value,&end,10);
                budget=(size_t)mib*1024*1024;
                if(*end || !mib || mib>10000000 || budget/1024/1024!=mib) {
                    fputs("Invalid FA18_LOOP_TRACE_BUDGET_MIB\n",stderr);exit(2);
                }
            }
            if(!fa18_flight_trace_open(&flight_trace,path,budget)) exit(2);
        }
    }
    if(flight_trace.file) {
        const uint8_t *viewport=trace_bytes(machine,0xc18242u,4);
        const unsigned width=(unsigned)viewport[0]<<8|viewport[1];
        const unsigned height=(unsigned)viewport[2]<<8|viewport[3];
        if(!fa18_flight_trace_write(&flight_trace,(unsigned)iteration,(unsigned)frame,
            width,height,trace_bytes,machine)) exit(2);
    }
}
void fa18_loop_iteration(void) {
    FA18Machine *m = fa18_machine;
    iteration++;
    dump_if_asked(m);
    trace_if_asked(m);
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
