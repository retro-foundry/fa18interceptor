#ifndef FA18_NATIVE_SCENE_PLACEMENT_H
#define FA18_NATIVE_SCENE_PLACEMENT_H
#include "field_window.h"
#include "hunk.h"
#include "native_record_orientation.h"
#include "scene_player_setup.h"

typedef struct {
    uint16_t segment;
    uint32_t offset;
    PortFieldWindow data;
    /* Some original owners consume a reference field as a numeric operand.
     * It is never used to locate data. Bind its source value explicitly when
     * that operation is required; group copies keep it with the reference. */
    uint32_t carried_value;
    uint8_t carried_value_bound;
} FA18NativeAssetReference;
typedef struct {
    enum { FA18_SCENE_PROCEDURE_NONE, FA18_SCENE_PROCEDURE_COMPONENT_ACCUMULATION,
           FA18_SCENE_PROCEDURE_RECORD_STREAM } procedure;
    FA18NativeAssetReference data[4]; /* original pointers 1..4 */
} FA18NativeScenePointerGroup;
typedef struct {
    PortFieldWindow poses,grid_words,grid_bytes;
    FA18FlightTrigData trig;
    FA18NativeScenePointerGroup input_10,input_other;
} FA18NativeScenePlacementAssets;
typedef struct {
    uint8_t *bytes,*words;
    size_t byte_count,word_byte_count;
    size_t byte_cursor,word_cursor,playback_byte,playback_word;
    uint16_t record_count,playback_count;
} FA18NativeSceneRecorder;
typedef struct {
    FA18NativeScenePlayerSetup *player;
    FA18NativeScenePlacementAssets *assets;
    FA18NativeScenePointerGroup *pointer_groups;
    size_t pointer_group_count;
    FA18NativeSceneRecorder *recorder;
    uint16_t *context_record,*condition_key_a,*condition_key_b,*grid_origin_x,*grid_origin_z,*error_word;
    int32_t *target_point; /* three longs */
    uint8_t *root_ready,*bar_e_flag,*bar_redraw_e,*fire_state,*input_source;
} FA18NativeScenePlacement;

int fa18_load_native_scene_placement_assets(const FA18Hunks *hunks,
                                             FA18NativeScenePlacementAssets *assets);
/* Original hunk 16 descriptor row. Handler identity and data references
 * resolve relocations into the original source procedure/assets. */
int fa18_load_native_scene_pointer_group(const FA18Hunks *hunks,uint32_t offset,
                                          FA18NativeScenePointerGroup *group);
/* Complete $C09266-$C095BE. Original assets and mutable scene-pointer groups
 * remain caller-owned. Returns 0 when required data/owners are unavailable,
 * retaining source-ordered stores already completed. */
int fa18_place_native_scene_root(FA18NativeScenePlacement *state);
#endif
