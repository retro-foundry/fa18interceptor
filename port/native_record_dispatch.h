#ifndef FA18_NATIVE_RECORD_DISPATCH_H
#define FA18_NATIVE_RECORD_DISPATCH_H
#include "native_record_view.h"
#include "native_record_range.h"
#include "native_context_publication.h"

typedef struct {
    FA18NativeRecordView *view;
    FA18NativeRecordViewWork *view_work;
    FA18NativeRecordRange *range;
    FA18NativeContextPublication *publication;
    const PortFieldWindow *selected_controls; /* original 64-byte control rows */
    uint8_t *cell_only,*track_enable,*sequence_phase,*sequence_step,*detail_clear,*scene_redraw;
    uint8_t *control_choice[2];
    uint16_t *target_slot;
} FA18NativeRecordDispatch;

/* Complete C23A7E, with actual C24568 classification, C23CA6 view, and
 * C1BEE8/C1B7A6 publication owners. Only the view's fault remains a lower
 * dependency. Completion and source pose decision are distinct. */
int fa18_dispatch_native_record(FA18NativeRecordDispatch *state,unsigned slot,
                                  unsigned companion_slot,int *decision);
/* Complete original alternate entry C23F4A, including C243F2 tracking. */
int fa18_dispatch_native_unclassified_record(FA18NativeRecordDispatch *state,
    unsigned slot,int *decision);
/* Complete C243F2 tracking tail, including actual selected-control targeting. */
int fa18_track_native_record_target(FA18NativeRecordDispatch *state,unsigned slot);
/* Complete C24458 selected-control target body; C243F2 selects this from the
 * literal class-specific control choice, or copies the linked record point. */
int fa18_select_native_control_target(FA18NativeRecordDispatch *state,unsigned slot,
                                       uint8_t choice);
#endif
