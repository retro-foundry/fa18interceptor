#ifndef FA18_NATIVE_RECORD_VIEW_H
#define FA18_NATIVE_RECORD_VIEW_H
#include "native_scene_records.h"
#include "field_window.h"

typedef struct FA18NativeRecordView FA18NativeRecordView;
typedef struct {
    /* Actual C2574A normalization and C06C02 source fault boundaries. */
    int (*normalize)(void *context,FA18NativeRecordView *state,int16_t scale,
                     const int32_t components[3],int16_t output[3]);
    int (*fault)(void *context,FA18NativeRecordView *state);
    void *context;
} FA18NativeRecordViewOps;
typedef struct {
    PortFieldWindow parameters,status,primary_list;
    const PortFieldWindow *zones; size_t zone_count; /* box then ten-byte exits */
} FA18NativeRecordViewAssets;
typedef struct {
    FA18NativeSceneRecord *viewer;
    uint32_t carried_axis; /* source D4 input retained for the signed word gate */
} FA18NativeRecordViewWork;
struct FA18NativeRecordView {
    FA18NativeSceneRecords *records;
    const FA18NativeRecordViewAssets *assets;
    const FA18NativeRecordViewOps *ops;
    uint16_t *selected_record,*current_stride,*current_slot,*tick_word,*error_word;
    uint8_t *post_input_event,*mode,*limit,*pending,*view_flag,*created,*admitted;
    int16_t *normalized; /* actual shared three-word output of the normalizer */
};
/* Complete C23CA6 and all its internal shared tails, with direct C091E0
 * placement and C2436A in-sight behavior. Fault/normalization are explicit
 * true lower owners. No CPU, address space or machine services are used. */
int fa18_update_native_record_view(FA18NativeRecordView *state,unsigned slot,
                                    FA18NativeRecordViewWork *work);
#endif
