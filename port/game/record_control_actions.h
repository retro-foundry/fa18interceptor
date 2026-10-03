#ifndef FA18_RECORD_CONTROL_ACTIONS_H
#define FA18_RECORD_CONTROL_ACTIONS_H
#include "memory.h"
enum RecordActionChild {
    RA_CHECK_RECORD,RA_CHECK_GROUND,RA_DRAW_SPECIAL,RA_DRAW_TRACKED,RA_DRAW_STANDARD,
    RA_INITIALISE_DIRECTION,RA_TRANSFORM_DIRECTION,RA_PROJECT_DIRECTION,
    RA_RESERVE_ALERT,RA_START_ALERT
};
enum RecordActionPhase {
    RA_PRIMARY_BYTE,RA_PRIMARY_WORD,RA_PRIMARY_LONG,RA_SECONDARY_BYTE,RA_SECONDARY_WORD,RA_SECONDARY_LONG,
    RA_LOOKUP,RA_OTHER_RECORD,RA_HEIGHT_CURSOR,RA_NEXT_OTHER_RECORD,
    RA_STORE_BYTE,RA_STORE_WORD,RA_STORE_LONG,RA_TEST_WORD,RA_TEST_LONG,
    RA_COMPARE_BYTE,RA_COMPARE_WORD,RA_COMPARE_LONG,RA_BIT_TEST,
    RA_PRIMARY_EXT_LONG,RA_PRIMARY_AND_WORD,RA_PRIMARY_AND_LONG,RA_PRIMARY_OR_WORD,
    RA_PRIMARY_ADD_WORD,RA_PRIMARY_SUB_WORD,RA_PRIMARY_ADD_LONG,RA_PRIMARY_ASR_WORD,RA_PRIMARY_ASR_LONG,RA_PRIMARY_ASL_LONG,
    RA_SECONDARY_EXT_WORD,RA_SECONDARY_EXT_LONG,RA_SECONDARY_ADD_LONG,RA_SECONDARY_ASL_LONG,
    RA_MEMORY_ADD_WORD,RA_MEMORY_SUB_WORD,RA_MEMORY_ADD_LONG,RA_MEMORY_SUB_LONG,RA_MEMORY_NEG_WORD,
    RA_ORIENTATION_BASE,RA_LAUNCH_POSITION,RA_LAUNCH_VELOCITY,RA_TARGET_RELATIVE,RA_DIRECTION_ARGUMENTS
};
typedef struct {
    uint32_t (*consume)(void *context,enum RecordActionChild child);
    void (*observe)(void *context,enum RecordActionPhase phase,uint32_t value,uint32_t other);
    void *context;
} RecordActionHooks;
void advance_control_record_action(gaddr frame,const RecordActionHooks *h);
void initialise_control_record_action(gaddr frame,const RecordActionHooks *h);
void aim_control_record_action(gaddr frame,const RecordActionHooks *h);
void publish_control_record_direction(gaddr frame,const RecordActionHooks *h);
void start_control_record_alert(gaddr frame,const RecordActionHooks *h);
void attenuate_control_record_offset(gaddr frame,const RecordActionHooks *h);
#endif
