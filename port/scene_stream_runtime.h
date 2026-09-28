#ifndef FA18_SCENE_STREAM_RUNTIME_H
#define FA18_SCENE_STREAM_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#include "record_walker_prefix.h"
#include "scene_stream_entry.h"
#include "transform.h"

typedef enum {
    FA18_SCENE_STREAM_RUNTIME_RETURN_ZERO,
    FA18_SCENE_STREAM_RUNTIME_RETURN_ONE,
    FA18_SCENE_STREAM_RUNTIME_WALKER_COMPLETE,
    FA18_SCENE_STREAM_RUNTIME_UNPORTED_CONTROL_BRANCH
} FA18SceneStreamRuntimeRoute;

/* Caller-owned mutable counterpart of `$C48390`.  The byte layout stays
 * big-endian because `$C1F6F8` and `$C2005C` address it by original offsets. */
typedef struct {
    uint8_t *bytes;
    size_t byte_count;
} FA18SceneStreamTransformedWorkspace;

/* Source inputs spanning `$C1EE14-$C1F6F8`'s direct raw-triple branch.  The
 * caller supplies the already-published `$C45A62`, `$C45B30/$34/$38`,
 * `$C45AB8`, and `$C45BD8` values; this owner preserves their P-code order
 * and does not choose any scene data or scheduling. */
typedef struct {
    FA18SceneStreamEntryInput entry;
    int32_t prepared_component[3]; /* `$C45A62/$66/$6A` before C1F464. */
    int32_t stream_component_x; /* `$C45B30` */
    int32_t stream_component_y; /* `$C45B34` */
    int32_t stream_component_z; /* `$C45B38` */
    uint16_t stream_shift; /* `$C45AB8` */
    FA18TransformMatrix matrix; /* `$C45BD8` */
    FA18SceneStreamTransformedWorkspace workspace;
    uint32_t walker_step_budget;
    FA18RecordWalkerTripleHandler triple_handler;
    FA18RecordWalkerHexHandler hex_handler;
    FA18RecordWalkerOtherHandler other_handler;
    void *handler_context;
} FA18SceneStreamRuntimeInput;

typedef struct {
    FA18SceneStreamEntryResult entry;
    FA18SceneStreamDescriptorFields descriptor;
    FA18VertexTransform transform;
    uint16_t transformed_vertex_count;
    FA18RecordWalkerPrefixResult walker;
} FA18SceneStreamRuntimeResult;

/* Compose the source's selected direct branch only:
 * `$C1EE14-$C1EF9C -> $C1F464-$C1F578 -> $C1F6F8-$C1F79F`.
 * Bit-0 descriptor branches and bit-1 alternate transforms are reported
 * explicitly rather than substituted. */
int fa18_run_scene_stream_direct_runtime(
    const FA18SceneStreamRuntimeInput *input,
    FA18SceneStreamRuntimeResult *result,
    FA18SceneStreamRuntimeRoute *route);

#endif
