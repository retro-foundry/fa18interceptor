#ifndef FA18_NATIVE_REPLAY_H
#define FA18_NATIVE_REPLAY_H
#include "../game/native/frontend.h"
#include <stddef.h>
typedef struct { unsigned iteration;uint8_t key,down; } NativeReplayEvent;
typedef struct {
    NativeReplayEvent *events;
    size_t count,next;
    unsigned iteration,end;
    int started;
} NativeReplay;
/* FA18_LOOP_INPUT_V1 or FA18_GAME_INPUT_V1; frame is informational. Loop rows
 * are injected edges; game rows are consumed edges for CPU-free comparisons.
 * Anchor at the first main-menu update following native cold startup. */
int native_replay_load(NativeReplay *replay,const char *path,char *error,size_t capacity);
void native_replay_update(NativeFrontend *game,void *context);
void native_replay_close(NativeReplay *replay);
#endif
