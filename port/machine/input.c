#include "input.h"
#include "../amiga/host_keys.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int fa18_amiga_rawkey(int k) { return amiga_host_raw_key(k); }
int fa18_amiga_rawkey_retro(int k) { return amiga_host_legacy_raw_key(k); }

int fa18_replay_load(FA18Replay *replay, const char *path) {
    FILE *f = fopen(path, "r");
    char line[256];
    int capacity = 0;
    memset(replay, 0, sizeof *replay);
    if (!f) return 0;
    if (!fgets(line, sizeof line, f) || strncmp(line, "E9K_INPUT_V1", 12)) {
        fclose(f);
        return 0;
    }
    while (fgets(line, sizeof line, f)) {
        FA18ReplayEvent e;
        char kind;
        int n;
        memset(&e, 0, sizeof e);
        n = sscanf(line, "F %d %c %d %d %d %d", &e.frame, &kind, &e.a, &e.b, &e.c, &e.d);
        if (n < 2) continue;
        e.kind = kind;
        if (replay->count == capacity) {
            FA18ReplayEvent *grown;
            capacity = capacity ? capacity * 2 : 256;
            grown = realloc(replay->events, (size_t)capacity * sizeof *grown);
            if (!grown) { fclose(f); return 0; }
            replay->events = grown;
        }
        replay->events[replay->count++] = e;
    }
    fclose(f);
    return 1;
}

void fa18_replay_free(FA18Replay *replay) {
    free(replay->events);
    memset(replay, 0, sizeof *replay);
}

void fa18_replay_apply(FA18Replay *replay, FA18Machine *m, int frame) {
    while (replay->next < replay->count && replay->events[replay->next].frame <= frame) {
        const FA18ReplayEvent *e = &replay->events[replay->next++];
        if (e->frame < frame) continue;
        switch (e->kind) {
        case 'K': {
            int raw = fa18_amiga_rawkey_retro(e->a);
            if (raw >= 0) fa18_machine_key(m, raw, e->d);
            break;
        }
        case 'm':
            if (e->a == 0 || e->a == 4) fa18_machine_mouse(m, e->b, e->c);
            break;
        case 'b':
            if (e->a == 0 || e->a == 4) fa18_machine_button(m, e->b, e->c);
            break;
        case 'J':
            if (e->a == 0 && e->b >= 0 && e->b < 16) {
                replay->pad[e->b] = e->c != 0;
                fa18_machine_joystick(m, replay->pad[4], replay->pad[5], replay->pad[6], replay->pad[7]);
                fa18_machine_button(m, 2, replay->pad[0]);
            }
            break;
        case 'C':
            memset(replay->pad, 0, sizeof replay->pad);
            fa18_machine_joystick(m, 0, 0, 0, 0);
            fa18_machine_button(m, 2, 0);
            break;
        default: break;
        }
    }
}
