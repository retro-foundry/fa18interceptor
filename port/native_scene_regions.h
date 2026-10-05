#ifndef FA18_NATIVE_SCENE_REGIONS_H
#define FA18_NATIVE_SCENE_REGIONS_H
#include "native_scene_placement.h"
#include "native_record_view.h"

/* A signed source row selector identifies an ordinary descriptor owner. It is
 * a table key, never a runtime address. Groups may alias the mutable bank.
 * The third descriptor field is also consumed as a numeric carried operand
 * before rejection; bind that scalar explicitly, separately from references.
 * No address is reconstructed or dereferenced by this owner. */
typedef struct {
    int16_t selector;
    FA18NativeScenePointerGroup *group;
} FA18NativeRegionDescriptor;
typedef struct {
    const PortFieldWindow *parameters;
    const PortFieldWindow *directory; /* NULL bytes is the source terminator */
    size_t directory_count;
    const FA18NativeRegionDescriptor *descriptors;
    size_t descriptor_count;
    FA18FlightTrigData trig;
} FA18NativeSceneRegionAssets;
typedef struct {
    FA18NativeSceneRecords *records;
    FA18NativeRecordViewWork *view_work;
    const FA18NativeSceneRegionAssets *assets;
    FA18NativeScenePointerGroup *pointer_groups;
    size_t pointer_group_count;
    uint8_t *recorder_on,*occupied,*mode,*admitted,*active,*reuse_jitter;
    uint16_t *random_word,*jitter_x,*jitter_z;
} FA18NativeSceneRegions;

/* Complete $C28F16; the supplied values are source MOVEM.W coordinates and
 * its already-extended altitude. All stores use the live record field view. */
int fa18_set_native_region_view(FA18NativeSceneRecord *record,
    const uint16_t coordinates[4],uint32_t altitude);
/* Original Hunk-27 relative parameters and relocated region directory.
 * Storage and descriptor bindings remain caller-owned. Region windows retain
 * live original data; numeric reference operands need explicit source bindings. */
int fa18_load_native_scene_region_assets(const FA18Hunks *hunks,
    FA18NativeSceneRegionAssets *assets,PortFieldWindow *parameters,
    PortFieldWindow *directory,size_t capacity,
    const FA18NativeRegionDescriptor *descriptors,size_t descriptor_count);
/* Complete $C28996. Directory traversal and occupied bits use the original
 * index mutated by spawning, rather than an independent eight-row counter. */
int fa18_update_native_scene_regions(FA18NativeSceneRegions *state);
/* Complete $C28B16/$C28B34. source_index is numeric region/order state. A bare
 * dispatch counter denotes counter+1 rows, including 65536 for $FFFF. */
int fa18_spawn_native_region_records(FA18NativeSceneRegions *state,
    const PortFieldWindow *region,uint32_t *source_index);
int fa18_dispatch_native_region_records(FA18NativeSceneRegions *state,
    const PortFieldWindow *rows,uint16_t counter,uint32_t *source_index);
#endif
