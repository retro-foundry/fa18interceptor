#ifndef FA18_NATIVE_REPLAY_H
#define FA18_NATIVE_REPLAY_H
#include "../game/native/frontend.h"
#include <stddef.h>
typedef struct { unsigned iteration;uint8_t key,down; } NativeReplayEvent;
enum { NATIVE_REPLAY_ANCHOR_CAPACITY=32 };
typedef struct {
    unsigned source_first,mode,stage,game_tick;
    unsigned native_first,frame;
} NativeReplayAnchor;
typedef struct {
    NativeReplayEvent *events;
    size_t count,next;
    unsigned iteration,end;
    int started;
    /* Optional comparison bookkeeping; no gameplay storage or clock writes.
     * iteration always counts actual native updates, including every wait. */
    NativeReplayAnchor anchors[NATIVE_REPLAY_ANCHOR_CAPACITY];
    unsigned anchor_count,anchor_active,source_iteration;
    int anchor_complete,anchor_failed;
} NativeReplay;
/* FA18_LOOP_INPUT_V1 or FA18_GAME_INPUT_V1; frame is informational. Loop rows
 * are injected edges; game rows are consumed edges for CPU-free comparisons.
 * Anchor at the first main-menu update following native cold startup. */
int native_replay_load(NativeReplay *replay,const char *path,char *error,size_t capacity);
int native_replay_load_anchors(NativeReplay *replay,const char *path,char *error,size_t capacity);
int native_replay_write_anchors(const NativeReplay *replay,const char *path);
void native_replay_update(NativeFrontend *game,void *context);
void native_replay_close(NativeReplay *replay);
#endif
