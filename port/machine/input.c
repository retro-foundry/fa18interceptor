#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fa18_amiga_rawkey(int k) {
    static const char row0[] = "`1234567890-=\\";
    static const char row1[] = "qwertyuiop[]";
    static const char row2[] = "asdfghjkl;'";
    static const char row3[] = "zxcvbnm,./";
    const char *p;
    if (k >= 'A' && k <= 'Z') k += 'a' - 'A';
    if (k > 0 && k < 128) {
        if ((p = strchr(row0, k)) != NULL) return (int)(p - row0);
        if ((p = strchr(row1, k)) != NULL) return 0x10 + (int)(p - row1);
        if ((p = strchr(row2, k)) != NULL) return 0x20 + (int)(p - row2);
        if ((p = strchr(row3, k)) != NULL) return 0x31 + (int)(p - row3);
    }
    switch (k) {
    case ' ': return 0x40;
    case '\b': return 0x41;
    case '\t': return 0x42;
    case '\r': return 0x44;
    case 27: return 0x45;
    case 127: return 0x46;
    /* SDLK_* values for non-character keys are scancode | 1<<30. */
    case 0x40000052: return 0x4C; /* up */
    case 0x40000051: return 0x4D; /* down */
    case 0x4000004F: return 0x4E; /* right */
    case 0x40000050: return 0x4F; /* left */
    case 0x400000E1: return 0x60; /* left shift */
    case 0x400000E5: return 0x61; /* right shift */
    case 0x40000039: return 0x62; /* caps lock */
    case 0x400000E0: case 0x400000E4: return 0x63; /* ctrl */
    case 0x400000E2: return 0x64; /* left alt */
    case 0x400000E6: return 0x65; /* right alt */
    case 0x400000E3: return 0x66; /* left GUI = left Amiga */
    case 0x400000E7: return 0x67; /* right GUI = right Amiga */
    case 0x40000049: return 0x5F; /* insert = Help */
    case 0x40000062: return 0x0F; /* keypad 0 */
    case 0x40000059: return 0x1D;
    case 0x4000005A: return 0x1E;
    case 0x4000005B: return 0x1F;
    case 0x4000005C: return 0x2D;
    case 0x4000005D: return 0x2E;
    case 0x4000005E: return 0x2F;
    case 0x4000005F: return 0x3D;
    case 0x40000060: return 0x3E;
    case 0x40000061: return 0x3F; /* keypad 9 */
    case 0x40000063: return 0x3C; /* keypad . */
    case 0x40000056: return 0x4A; /* keypad - */
    case 0x40000058: return 0x43; /* keypad enter */
    case 0x40000054: return 0x5C; /* keypad / */
    case 0x40000055: return 0x5D; /* keypad * */
    case 0x40000057: return 0x5E; /* keypad + */
    default: break;
    }
    if (k >= 0x4000003A && k <= 0x40000043) return 0x50 + (k - 0x4000003A); /* F1-F10 */
    return -1;
}

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
            int raw = fa18_amiga_rawkey(e->a);
            if (raw >= 0) fa18_machine_key(m, raw, e->d);
            break;
        }
        case 'm':
            if (e->a == 0 || e->a == 4) fa18_machine_mouse(m, e->b, e->c);
            break;
        case 'b':
            if (e->a == 0 || e->a == 4) fa18_machine_button(m, e->b, e->c);
            break;
        default: break;
        }
    }
}
