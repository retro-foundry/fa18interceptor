#ifndef FA18_NATIVE_RECORD_VIEW_H
#define FA18_NATIVE_RECORD_VIEW_H
#include "native_scene_records.h"
#include "field_window.h"
#include "native_vector_math.h"

typedef struct FA18NativeRecordView FA18NativeRecordView;
typedef struct {
    /* Actual C06C02 source fault boundary. */
    int (*fault)(void *context,FA18NativeRecordView *state,uint32_t *axis);
    void *context;
} FA18NativeRecordViewOps;
typedef struct {
    PortFieldWindow parameters,status,primary_list;
    const PortFieldWindow *zones; size_t zone_count; /* box then ten-byte exits */
} FA18NativeRecordViewAssets;
typedef struct {
    FA18NativeSceneRecord *viewer;
    uint32_t carried_axis; /* source D4 input/output across actual lower owners */
    FA18NativeSceneRecord *companion; /* live source A2 record identity */
} FA18NativeRecordViewWork;
struct FA18NativeRecordView {
    FA18NativeSceneRecords *records;
    const FA18NativeRecordViewAssets *assets;
    const FA18NativeRecordViewOps *ops;
    const FA18NativeVectorMath *vector_math;
    uint16_t *selected_record,*current_stride,*current_slot,*tick_word,*error_word;
    uint8_t *post_input_event,*mode,*limit,*pending,*view_flag,*created,*admitted;
    int16_t *normalized; /* actual shared three-word output of the normalizer */
};
/* Complete C23CA6 and all its internal shared tails, with direct C091E0
 * placement and C2436A in-sight behavior. Normalization runs actual C2574A/C1D974; fault handling remains an
 * explicit true lower owner. No CPU, address space or machine services are used. */
int fa18_update_native_record_view(FA18NativeRecordView *state,unsigned slot,
                                    FA18NativeRecordViewWork *work);
/* Actual C23FF8 shared selected-reference tail used by dispatch. */
int fa18_link_native_record_view(FA18NativeRecordView *state,unsigned slot,
                                  FA18NativeRecordViewWork *work);
#endif
