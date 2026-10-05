#ifndef FA18_NATIVE_RECORD_ACTION_PLACEMENT_H
#define FA18_NATIVE_RECORD_ACTION_PLACEMENT_H
#include "native_scene_placement.h"
#include "native_record_view.h"

typedef struct {
    FA18NativeScenePointerGroup primary,alternate;
    PortFieldWindow offsets_10,offsets_other;
} FA18NativeRecordActionPlacementAssets;
typedef struct {
    FA18NativeSceneRecords *records;
    const FA18NativeRecordActionPlacementAssets *assets;
    FA18NativeScenePointerGroup *pointer_groups;
    size_t pointer_group_count;
    FA18NativeRecordViewWork *view_work;
    const PortFieldByte *viewed_record; /* logical word pair; live viewed identity */
    uint16_t *current_slot,*selector_word,*primary_count,*alternate_count;
    uint32_t *warning_causes,*events;
    uint8_t *space_latch,*fire_pending,*secondary_enable,*limit,*divisor,
        *alert_countdown,*fire_state,*mode_changed,*stores_redraw_a,*stores_redraw_b,*scene_redraw;
} FA18NativeRecordActionPlacement;

int fa18_load_native_record_action_placement_assets(const FA18Hunks *hunks,
                                                     FA18NativeRecordActionPlacementAssets *assets);
/* The loader resolves descriptor references and binds the unchanged table
 * words through the first adjacent relocation. Larger store indices require
 * caller-bound `after` fields for the original adjacent numeric values. */
/* Complete $C2374C / $C2377E entries, including the shared placement tail.
 * Destination and companion may alias. No child calls are reachable from
 * these entries: both set kind to 0/1 before the shared class-$30 tests.
 * Failure retains preceding stores. */
int fa18_place_native_primary_record(FA18NativeRecordActionPlacement *state,
                                       unsigned slot,unsigned companion);
int fa18_place_native_secondary_record(FA18NativeRecordActionPlacement *state,
                                         unsigned slot,unsigned companion);
#endif
