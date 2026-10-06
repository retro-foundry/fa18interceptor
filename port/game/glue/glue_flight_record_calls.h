#ifndef FA18_GLUE_FLIGHT_RECORD_CALLS_H
#define FA18_GLUE_FLIGHT_RECORD_CALLS_H
/* Parent-selected C calls; no child CPU entry registrations. */
int glue_schedule_record_input(void);
int glue_schedule_indexed_record(void);
int glue_schedule_record_action(void);
/* Held original-child fixture entry, separate from the live continuation. */
int glue_record_action_reference(void);
#endif
