#include "workspace_segment_projection.h"

#include <limits.h>

static int project_component(int16_t component, int16_t depth, int16_t scale,
                             int16_t center, int16_t maximum, int16_t *result) {
    const int32_t product = (int32_t)component * scale;
    const int32_t quotient = product / depth;
    int32_t projected;

    /* DIVS.W stores the quotient in a word.  The complete overflow route is
     * not reached by this isolated source entry, so do not silently wrap a
     * host result into a different endpoint. */
    if (quotient < INT16_MIN || quotient > INT16_MAX) return -1;
    projected = quotient + center;
    if (projected < 0) projected = 0;
    else if (projected >= maximum + 1) projected = maximum;
    *result = (int16_t)(maximum - projected);
    return 0;
}

static int16_t negate_word(int16_t value) {
    return (int16_t)(UINT16_C(0) - (uint16_t)value);
}

static int project_endpoint(const FA18ViewVertex *endpoint,
                            FA18ScreenPoint *screen) {
    int16_t x, y;

    /* `$C2ED76-$C2EDAC` / `$C2EDCE-$C2EE04`: the source rejects positive
     * x/y magnitudes strictly greater than depth before division. */
    if (endpoint->depth <= 0 || endpoint->x > endpoint->depth ||
        negate_word(endpoint->x) > endpoint->depth || endpoint->y > endpoint->depth ||
        negate_word(endpoint->y) > endpoint->depth)
        return 0;
    if (project_component(endpoint->x, endpoint->depth, 160, 160, 319, &x) != 0 ||
        project_component(endpoint->y, endpoint->depth, 90, 90, 179, &y) != 0)
        return -1;
    screen->x = x;
    screen->y = y;
    return 1;
}

int fa18_project_workspace_segment_to_line(
    const FA18ViewVertex endpoints[2], FA18WorkspaceSegmentLineEmitter emitter,
    void *emitter_context) {
    FA18ScreenPoint projected[2];
    int status;

    if (!endpoints || !emitter) return -1;
    status = project_endpoint(&endpoints[0], &projected[0]);
    if (status <= 0) return status;
    status = project_endpoint(&endpoints[1], &projected[1]);
    if (status <= 0) return status;
    if (emitter(emitter_context, projected[0].x, projected[0].y,
                projected[1].x, projected[1].y) != 0)
        return -1;
    return 1;
}
