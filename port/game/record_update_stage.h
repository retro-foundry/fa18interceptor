#ifndef FA18_GAME_RECORD_UPDATE_STAGE_H
#define FA18_GAME_RECORD_UPDATE_STAGE_H
#include "memory.h"

enum RecordUpdateChild {
    RECORD_UPDATE_PERIODIC, RECORD_UPDATE_RELEASE_SELECTION,
    RECORD_UPDATE_ROOT_CONTROL, RECORD_UPDATE_ROOT_VIEW, RECORD_UPDATE_ROOT_MARKER,
    RECORD_UPDATE_POSE, RECORD_UPDATE_PRIMARY_READY, RECORD_UPDATE_PRIMARY_PLACE,
    RECORD_UPDATE_SECONDARY_READY, RECORD_UPDATE_SECONDARY_PLACE,
    RECORD_UPDATE_SECONDARY_CONTROL, RECORD_UPDATE_PAIRED_READY,
    RECORD_UPDATE_DISPATCH, RECORD_UPDATE_FINISH
};
enum RecordUpdatePhase {
    RECORD_UPDATE_SAVE, RECORD_UPDATE_COUNTDOWNS, RECORD_UPDATE_PERIODIC_GATE,
    RECORD_UPDATE_RELEASE_GATE, RECORD_UPDATE_ROOT, RECORD_UPDATE_SLOT,
    RECORD_UPDATE_ACTIVE_BIT, RECORD_UPDATE_GROUP_GATE, RECORD_UPDATE_RESTORE
};
typedef struct {
    enum RecordUpdatePhase phase;
    unsigned slot;
    uint32_t value, other;
    gaddr record, companion;
} RecordUpdateEvent;
typedef struct {
    /* Ready/dispatch children return the original nonzero decision, whose
     * representation belongs to the CPU adapter. All child effects stay live. */
    int (*consume)(void *context, enum RecordUpdateChild child, unsigned slot);
    void (*observe)(void *context, const RecordUpdateEvent *event);
    void *context;
} RecordUpdateHooks;
enum ControlRecordsPhase {
    CONTROL_RECORDS_BEGIN, CONTROL_RECORDS_AFTER_PERIODIC,
    CONTROL_RECORDS_AFTER_RELEASE, CONTROL_RECORDS_AFTER_ROOT_CONTROL,
    CONTROL_RECORDS_AFTER_ROOT_VIEW, CONTROL_RECORDS_AFTER_ROOT_MARKER,
    CONTROL_RECORDS_AFTER_ROOT_POSE, CONTROL_RECORDS_SLOTS,
    CONTROL_RECORDS_AFTER_FINISH, CONTROL_RECORDS_COMPLETE
};
enum ControlRecordSlotPhase {
    CONTROL_SLOT_BEGIN, CONTROL_SLOT_AFTER_READY, CONTROL_SLOT_AFTER_PLACE,
    CONTROL_SLOT_AFTER_CONTROL,
    CONTROL_SLOT_AFTER_DISPATCH, CONTROL_SLOT_AFTER_POSE
};
typedef struct {
    enum ControlRecordsPhase phase;
    enum ControlRecordSlotPhase slot_phase;
    enum RecordUpdateChild child;
    unsigned slot;
    int child_result;
} ControlRecordsFrame;
/* Zero requests the named child; one completes the retained C update. */
int advance_control_records(ControlRecordsFrame *frame,const RecordUpdateHooks *hooks);
/* Complete C22C80-C230AE parent, including inactive routes and slot 7's
 * preparation-only sequence. Child ownership remains explicit. */
void update_control_records(const RecordUpdateHooks *hooks);
#endif
