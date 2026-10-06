#ifndef FA18_GLUE_FLIGHT_RECORD_CALLS_H
#define FA18_GLUE_FLIGHT_RECORD_CALLS_H
#include "flight_dynamics.h"
#include "record_update_stage.h"
typedef struct {
    AutopilotFrame frame;
    uint32_t return_pc,return_sp;
    int started,original_transfer;
} NativeAutopilotCall;
typedef struct { ZoneExitFrame frame; int started; } NativeZoneExitCall;
int glue_continue_record_action(const void *arguments);
int glue_continue_record_zone_exit(const void *arguments);
int glue_complete_native_record_input(void);
int glue_complete_native_indexed_record(void);
int glue_complete_native_record_matrix(void);
typedef struct {
    RecordDynamicsFrame frame;
    NativeAutopilotCall action;
    NativeZoneExitCall zone;
    enum { CHILD_IDLE,CHILD_ORIGINAL,CHILD_ACTION,CHILD_ZONE,CHILD_FINISHED } active;
} NativeRecordDynamicsCall;
void glue_begin_record_dynamics(NativeRecordDynamicsCall *call);
int glue_continue_record_dynamics(const void *arguments);
int glue_schedule_record_dynamics(void);
typedef struct {
    ControlRecordsFrame frame;
    NativeRecordDynamicsCall dynamics;
    enum { RECORD_CHILD_IDLE,RECORD_CHILD_ORIGINAL,RECORD_CHILD_DYNAMICS,RECORD_CHILD_FINISHED } active;
} NativeControlRecordsCall;
void glue_begin_control_records(NativeControlRecordsCall *call);
int glue_continue_control_records(const void *arguments);
/* Parent-selected C calls; no child CPU entry registrations. */
int glue_schedule_record_input(void);
int glue_schedule_indexed_record(void);
int glue_schedule_record_action(void);
int glue_schedule_record_zone_exit(void);
/* Held original-child fixture entry, separate from the live continuation. */
int glue_record_action_reference(void);
#endif
