#ifndef FA18_SCENE_PLACEMENT_H
#define FA18_SCENE_PLACEMENT_H

#include <stddef.h>
#include <stdint.h>

/* `$C1CB74-$C1CCB6` walks either `$C4E9AA` or `$C4F03A` in 24-byte steps.
 * These are mutable placement records; they are not static model data. */
enum { FA18_SCENE_PLACEMENT_BYTES = 24 };

typedef struct {
    int16_t selector;
    uint32_t descriptor_reference;
    int16_t coordinate[3];
    uint16_t field_0c;
    uint16_t per_frame[5];
} FA18ScenePlacementRecord;

/* The selector only reads these descriptor words before handing the selected
 * placement to the type-specific route at `$C1CC50`/`$C1CCBC`. */
typedef struct {
    int16_t first_word;
    uint32_t first_target_hunk;
    uint32_t first_target_offset;
} FA18ScenePlacementDescriptorProbe;

typedef struct {
    uint8_t selector_byte;
    uint16_t selector_low_nibble;
    int32_t projection_coordinate[3];
    uint16_t next_offset;
    uint16_t accepted_count;
    uint16_t skipped_count;
} FA18ScenePlacementTraversalState;

typedef int (*FA18ScenePlacementDescriptorLookup)(void *context,
                                                   uint32_t descriptor_reference,
                                                   FA18ScenePlacementDescriptorProbe *probe);
typedef int (*FA18ScenePlacementConsumer)(void *context,
                                          const FA18ScenePlacementRecord *record,
                                          const FA18ScenePlacementDescriptorProbe *probe,
                                          const FA18ScenePlacementTraversalState *state);

typedef int (*FA18ScenePlacementDepthLookup)(void *context, int16_t index,
                                             int8_t *value);

/* Direct state produced by `$C1CB14-$C1CB73` for the following placement loop. */
typedef struct {
    uint8_t use_alternate_table;
    uint16_t initial_offset;
    uint8_t auxiliary_flag;
    uint8_t status_flag;
    uint16_t comparison_word;
} FA18ScenePlacementSelectorState;

/* `$C1CB14-$C1CB73`: select a placement-list offset and derive the signed
 * comparison word from the caller-owned depth byte table. */
int fa18_prepare_scene_placement_selector(
    uint8_t select_alternate_table, uint16_t primary_offset,
    uint16_t alternate_offset, int32_t projection_depth,
    FA18ScenePlacementDepthLookup depth_lookup, void *context,
    FA18ScenePlacementSelectorState *state);

typedef struct {
    uint8_t select_alternate_table;
    uint16_t primary_offset;
    uint16_t alternate_offset;
    int32_t projection_depth;
    int32_t gate_coordinate;
    const uint8_t *primary_table;
    size_t primary_size;
    const uint8_t *alternate_table;
    size_t alternate_size;
    FA18ScenePlacementDepthLookup depth_lookup;
    void *depth_context;
    FA18ScenePlacementDescriptorLookup descriptor_lookup;
    FA18ScenePlacementConsumer consumer;
    void *traversal_context;
} FA18ScenePlacementStageInput;

typedef struct {
    FA18ScenePlacementSelectorState selector;
    FA18ScenePlacementTraversalState traversal;
} FA18ScenePlacementStageResult;

/* `$C1CB14-$C1CCB9`: compose the selector prefix with the common record loop. */
int fa18_run_scene_placement_stage(const FA18ScenePlacementStageInput *input,
                                   FA18ScenePlacementStageResult *result);

typedef enum {
    FA18_PARENT_FLIGHT_PLACEMENT_PRIMARY_ENTRY,
    FA18_PARENT_FLIGHT_PLACEMENT_ALTERNATE_ENTRY
} FA18ParentFlightPlacementEntry;

/* Adapter for either `$C1CB14` or `$C1CB26` callback slot in the parent
 * update. The entry selects the source's fixed primary/alternate path. */
typedef struct {
    const FA18ScenePlacementStageInput *input;
    FA18ScenePlacementStageResult *result;
    FA18ParentFlightPlacementEntry entry;
} FA18ParentFlightPlacementStageContext;

int fa18_run_parent_flight_placement_stage(void *context);

/* Decode one big-endian runtime record. */
int fa18_decode_scene_placement_record(const uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES],
                                       FA18ScenePlacementRecord *record);

/* Bounded port of the common `$C1CB74` loop. `initial_offset` is the mutable
 * `$C459AA` word relative to the selected table. `use_alternate_table` picks
 * the `$C4F03A` family, but both table inputs are caller-owned byte streams.
 * `gate_coordinate` is the source's `$C45A66` comparison value.
 *
 * It deliberately ends at the descriptor-specific branch: the callback is
 * the native owner of the unresolved `$C1CC50` / `$C1CCBC` routes. */
int fa18_traverse_scene_placements(const uint8_t *primary_table,
                                   size_t primary_size,
                                   const uint8_t *alternate_table,
                                   size_t alternate_size,
                                   int use_alternate_table,
                                   uint16_t initial_offset,
                                   int32_t gate_coordinate,
                                   FA18ScenePlacementDescriptorLookup lookup,
                                   FA18ScenePlacementConsumer consumer,
                                   void *context,
                                   FA18ScenePlacementTraversalState *state);

#endif
