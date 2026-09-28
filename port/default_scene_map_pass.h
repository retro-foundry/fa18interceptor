#ifndef FA18_DEFAULT_SCENE_MAP_PASS_H
#define FA18_DEFAULT_SCENE_MAP_PASS_H

#include "default_scene_render_pass.h"
#include "map_packet_parent_pass.h"

/* Caller supplies the still-live map selectors, detail state, static Hunk
 * binding, and page submission in `map`.  This boundary supplies only the
 * values owned by the observed default scene route: the selected record's
 * root longs, `$C45BD8`, and `$C45A78`. */
typedef struct {
    FA18DefaultSceneRenderPassInput scene;
    FA18MapPacketParentPassInput map;
} FA18DefaultSceneMapPassInput;

typedef struct {
    FA18DefaultSceneRenderPassResult scene;
    FA18MapPacketParentPassResult map;
} FA18DefaultSceneMapPassResult;

/* Compose the default `$C2DB18 -> $C1C54E` state producer with the local
 * `$C2AA9C` map parent.  The source grid/page work remains in the default
 * pass; map geometry is submitted through the caller's map display callback.
 * Returns one when the default matrix route is not yet ported, zero on the
 * composed route, and minus one for invalid/bounded failures. */
int fa18_render_default_active_scene_map_pass(
    const FA18FlightTrigTable *trig_table, const FA18ProjectionGrid *grid,
    const FA18DefaultSceneMapPassInput *input,
    FA18FlightRendererPage *page_renderer,
    FA18MapPacketProjectionRecord *records, size_t record_capacity,
    FA18DefaultSceneMapPassResult *result);

#endif
