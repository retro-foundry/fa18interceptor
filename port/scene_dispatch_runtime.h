#ifndef FA18_SCENE_DISPATCH_RUNTIME_H
#define FA18_SCENE_DISPATCH_RUNTIME_H

#include <stdint.h>

#include "scene_dispatch_table.h"
#include "scene_negative_pose.h"
#include "scene_record_dispatch.h"

/* `$C46184-$C48383` contains seventeen complete 512-byte records.  The
 * adjacent `$C48390` workspace is deliberately not included. */
enum {
    FA18_SCENE_DISPATCH_RECORD_COUNT = 17,
    FA18_SCENE_DISPATCH_TEMPLATE_HUNK = 16,
    FA18_SCENE_DISPATCH_TEMPLATE_POINTERS = 5
};

/* One five-longword template copied by `$C28B96-$C28B9C` from Hunk 16 into
 * the record-indexed `$C22188` parameter pack. Only entries with a Hunk
 * relocation are resolved; the source carries the other pointer words as
 * opaque values and this port does not consume them. */
typedef struct {
    uint32_t segment[FA18_SCENE_DISPATCH_TEMPLATE_POINTERS];
    uint32_t offset[FA18_SCENE_DISPATCH_TEMPLATE_POINTERS];
} FA18SceneDispatchTemplate;

typedef struct {
    const FA18Hunks *hunks;
    FA18SceneDispatchRecord record[FA18_SCENE_DISPATCH_RECORD_COUNT];
    FA18SceneDispatchTemplate template_record[FA18_SCENE_DISPATCH_RECORD_COUNT];
} FA18SceneDispatchRuntime;

/* `$C28722-$C28E08`: select original Hunk-27 source records, publish their
 * Hunk-16 template references, and create only slots whose source bit-6 gate
 * is clear.  Mode `$7D` retains its separate source return as one. */
int fa18_initialize_scene_dispatch_runtime(
    const FA18Hunks *hunks, const FA18SceneDispatchTable *table,
    const FA18SceneDispatchSelectionInput *selection_input,
    int16_t inherited_d7, const FA18RecordMatrixUpdateOps *matrix_ops,
    FA18SceneDispatchRuntime *runtime);

/* Resolver adapters for `$C09498`: selected record data come from the
 * reconstructed mutable bank; descriptor field four is the source's
 * `+$10(A4,index*20)` parameter-table pointer. */
int fa18_scene_dispatch_runtime_resolve_negative_record(
    void *context, uint16_t record_index, FA18SceneNegativePoseRecord *record);
int fa18_scene_dispatch_runtime_resolve_negative_descriptor(
    void *context, uint16_t record_index, FA18SceneNegativePoseDescriptor *descriptor);

#endif
