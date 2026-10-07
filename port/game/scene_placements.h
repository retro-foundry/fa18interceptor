#ifndef FA18_GAME_SCENE_PLACEMENTS_H
#define FA18_GAME_SCENE_PLACEMENTS_H
#include "memory.h"

/* Complete C1CB14/C1CB26: traverse the primary/alternate 24-byte placement
 * list. Descriptor routines remain explicit child owners. No CPU state is
 * exposed by this domain boundary. Observations are synchronous; consume
 * is required, observe is optional. */
typedef struct {
    gaddr placement, descriptor, routine, parameters;
    uint16_t header;
    int16_t distance, kind;
    /* Caller calculation retained by childless descriptor exits. */
    int32_t prior_result;
} ScenePlacementCall;
enum ScenePlacementPhase {
    SCENE_PLACEMENT_SELECT, SCENE_PLACEMENT_SCAN, SCENE_PLACEMENT_DESCRIPTOR,
    SCENE_PLACEMENT_POSITION, SCENE_PLACEMENT_CACHE, SCENE_PLACEMENT_REFRESH_GATE,
    SCENE_PLACEMENT_DISTANCE_BEGIN, SCENE_PLACEMENT_DISTANCE_END,
    SCENE_PLACEMENT_CALL, SCENE_PLACEMENT_RESULT
};
typedef struct {
    enum ScenePlacementPhase phase;
    gaddr placement, descriptor;
    uint32_t value;
    uint16_t header;
    int16_t x, y, z;
    const ScenePlacementCall *call;
} ScenePlacementEvent;
typedef struct {
    int32_t (*consume)(void *context, const ScenePlacementCall *call);
    void (*observe)(void *context, const ScenePlacementEvent *event);
    void *context;
} ScenePlacementHooks;

void visit_scene_placements(int alternate, const ScenePlacementHooks *hooks);
#endif
