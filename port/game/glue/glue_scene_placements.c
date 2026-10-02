/* Complete C1CB14/C1CB26 CPU adapter. Descriptor consumers are independent
 * child calls; the domain owns the full list/refresh/skip/result contract. */
#include "glue.h"
#include "scene_placements.h"
#include "globals.h"
#include "glue_child_call.h"

void target_distance_registers(int apply, int32_t result);

static void placement_outputs(void *context, const ScenePlacementEvent *e) {
    (void)context;
    switch (e->phase) {
    case SCENE_PLACEMENT_SELECT:
        D(0) = e->value;
        if ((int32_t)e->value <= 0x7fff) {
            SET_W(D(0), e->x); A(2) = 0xc41130u;
        }
        break;
    case SCENE_PLACEMENT_SCAN:
        A(0) = e->placement + 2; SET_W(D(0), e->x); SET_W(D(7), e->header); break;
    case SCENE_PLACEMENT_DESCRIPTOR:
        SET_B(D(0), e->header); SET_W(D(0), D(0) & 15u);
        A(0) = e->placement + 6; A(1) = e->descriptor; break;
    case SCENE_PLACEMENT_POSITION:
        D(2) = (uint32_t)(int32_t)e->x << 8;
        D(3) = (uint32_t)(int32_t)e->y << 8;
        D(4) = (uint32_t)(int32_t)e->z << 8;
        A(0) = e->placement + 12; break;
    case SCENE_PLACEMENT_CACHE: D(1) = e->value; break;
    case SCENE_PLACEMENT_REFRESH_GATE:
        SET_W(D(0), e->value); SET_W(D(1), e->header); break;
    case SCENE_PLACEMENT_DISTANCE_BEGIN:
        D(2) = (uint32_t)(int32_t)e->x; D(3) = (uint32_t)(int32_t)e->y;
        D(4) = (uint32_t)(int32_t)e->z; break;
    case SCENE_PLACEMENT_DISTANCE_END:
        target_distance_registers(0, (int32_t)e->value); break;
    case SCENE_PLACEMENT_CALL:
        SET_W(D(1), e->call->distance); SET_W(D(7), e->call->kind);
        A(2) = e->call->routine; A(0) = e->call->parameters;
        A(1) = e->call->descriptor + 16;
        /* The final MOVE.W after LSR.W/EXT.W supplies callback N/Z/V/C;
         * X is the bit shifted out by the eight-bit logical right shift. */
        flags_logic_w(D(7)); FLAG_X = (e->call->header & 0x80u) << 1;
        break;
    case SCENE_PLACEMENT_RESULT:
        A(0) = e->placement; SET_W(D(1), e->x); D(0) = e->value; break;
    }
}

static int32_t placement_consumer(void *context, const ScenePlacementCall *call) {
    (void)context;
    return glue_complete_child(call->routine, 0xc1cc88u);
}

static int scene_placements(int alternate) {
    ScenePlacementHooks hooks = {placement_consumer, placement_outputs, NULL};
    visit_scene_placements(alternate, &hooks);
    return glue_return();
}
int glue_C1CB14(void) { return scene_placements(0); }
int glue_C1CB26(void) { return scene_placements(1); }
