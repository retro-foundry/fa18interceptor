#ifndef FA18_GAME_UPDATE_STAGE_H
#define FA18_GAME_UPDATE_STAGE_H
#include "memory.h"
enum UpdateStageChild { UPDATE_STAGE_RECORDS, UPDATE_STAGE_ORIGIN, UPDATE_STAGE_RATE };
enum UpdateStagePhase {
    UPDATE_STAGE_PREPARED, UPDATE_STAGE_RECORD_ROUTE, UPDATE_STAGE_ORIGIN_SAVE,
    UPDATE_STAGE_ORIGIN_RATE, UPDATE_STAGE_RECORD_KEYS, UPDATE_STAGE_ORIGIN_KEYS,
    UPDATE_STAGE_REQUEST_ALL, UPDATE_STAGE_DONE
};
typedef struct {
    enum UpdateStagePhase phase;
    uint32_t value, previous, coarse, flags;
    gaddr record;
    uint8_t requests;
} UpdateStageEvent;
typedef struct { gaddr record; uint8_t requests; } UpdateStageResult;
typedef struct {
    UpdateStageResult (*consume)(void *context,enum UpdateStageChild child);
    void (*observe)(void *context,const UpdateStageEvent *event);
    void *context;
} UpdateStageHooks;
enum RecordUpdateStagePhase {
    RECORD_STAGE_BEGIN, RECORD_STAGE_AFTER_RECORDS, RECORD_STAGE_AFTER_RATE,
    RECORD_STAGE_AFTER_ORIGIN, RECORD_STAGE_AFTER_ORIGIN_RATE, RECORD_STAGE_COMPLETE
};
typedef struct {
    enum RecordUpdateStagePhase phase;
    enum UpdateStageChild child;
    UpdateStageResult result;
    uint8_t requests;
} RecordUpdateStageFrame;
/* Zero requests a child; one finishes the retained game stage. */
int advance_record_update_stage(RecordUpdateStageFrame *frame,const UpdateStageHooks *hooks);
/* Complete C1C63E-C1C7F4 owner. Record update, origin and rate producers
 * remain explicit children; signed widths and publication order are source-owned. */
void run_record_update_stage(const UpdateStageHooks *hooks);
#endif
