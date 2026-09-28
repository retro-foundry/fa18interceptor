#include "projected_segment_preparation.h"

static int16_t neg_word(int16_t value) {
    return (int16_t)(UINT16_C(0) - (uint16_t)value);
}

static int project_component(int16_t value, int16_t depth, int16_t scale,
                             int16_t center, int16_t maximum, int16_t *screen) {
    int32_t projected;
    if (depth <= 0) return 0;
    projected = ((int32_t)value * scale) / depth + center;
    if (projected < 0) projected = 0;
    else if (projected >= maximum + 1) projected = maximum;
    *screen = (int16_t)(maximum - projected);
    return 1;
}

/* Literal branch translation of `$C2EE60-$C2F034`.  A return of one enters
 * `$C2F03A`; zero is `$C2EE44/$C2F034` rejection. */
static int select_components(FA18ProjectedSegmentPreparationState *state,
                             const FA18ViewVertex *current,
                             const FA18ViewVertex *other) {
    int outside;
#define CLIP(entry) \
    do { if (fa18_clip_projected_segment((FA18ViewVertex[2]){*current, *other}, \
                                         entry, &state->components, &outside) != 0) return 0; } while (0)

    if (current->x < current->depth) goto negative_d3;
    if (other->depth <= other->x) return 0;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D3_FIRST_C2F0C6);
    if (!outside) return 1;
    if (state->status_word >= 0) goto negative_d4;
    if (other->depth <= neg_word(other->x)) goto negative_d4;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D3_SECOND_C2F0F4);
    if (!outside) return 1;

negative_d4:
    if (current->y >= 0) goto nonnegative_d4;
    if (neg_word(current->y) < current->depth ||
        other->depth <= neg_word(other->y)) return 0;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D4_SECOND_C2F156);
    if (!outside) return 1;
    if (state->status_word >= 0 || current->y < current->depth ||
        other->depth <= other->y) return 0;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D4_FIRST_C2F128);
    return !outside;

nonnegative_d4:
    if (current->y < current->depth || other->depth <= other->y) return 0;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D4_FIRST_C2F128);
    if (!outside) return 1;
    if (state->status_word >= 0 || neg_word(current->y) < current->depth ||
        other->depth <= neg_word(other->y)) return 0;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D4_SECOND_C2F156);
    return !outside;

negative_d3:
    if (neg_word(current->x) < current->depth) goto d4_alternate;
    if (other->depth <= neg_word(other->x)) return 0;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D3_SECOND_C2F0F4);
    if (!outside) return 1;
    if (state->status_word >= 0) goto negative_d4;
    if (other->depth <= other->x) goto negative_d4;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D3_FIRST_C2F0C6);
    if (!outside) return 1;
    goto negative_d4;

d4_alternate:
    if (current->y >= current->depth) {
        if (other->depth <= other->y) return 0;
        CLIP(FA18_PROJECTED_SEGMENT_CLIP_D4_FIRST_C2F128);
        if (!outside) return 1;
        if (state->status_word >= 0 || other->depth <= neg_word(other->y))
            goto d3_completion;
        CLIP(FA18_PROJECTED_SEGMENT_CLIP_D4_SECOND_C2F156);
        if (!outside) return 1;
        goto d3_completion;
    }
    if (neg_word(current->y) < current->depth)
        return current->depth >= 0; /* `$C2F030-$C2F042` retained components. */
    if (other->depth <= neg_word(other->y)) return 0;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D4_SECOND_C2F156);
    if (!outside) return 1;
    if (state->status_word >= 0 || other->depth <= other->y) goto d3_completion;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D4_FIRST_C2F128);
    if (!outside) return 1;

d3_completion:
    if (current->x < 0) {
        if (neg_word(current->x) < current->depth ||
            other->depth <= neg_word(other->x)) return 0;
        CLIP(FA18_PROJECTED_SEGMENT_CLIP_D3_SECOND_C2F0F4);
        if (!outside) return 1;
        if (state->status_word >= 0 || current->x < current->depth ||
            other->depth <= other->x) return 0;
        CLIP(FA18_PROJECTED_SEGMENT_CLIP_D3_FIRST_C2F0C6);
        return !outside;
    }
    if (current->x < current->depth || other->depth <= other->x) return 0;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D3_FIRST_C2F0C6);
    if (!outside) return 1;
    if (state->status_word >= 0 || neg_word(current->x) < current->depth ||
        other->depth <= neg_word(other->x)) return 0;
    CLIP(FA18_PROJECTED_SEGMENT_CLIP_D3_SECOND_C2F0F4);
    return !outside;
#undef CLIP
}

int fa18_prepare_projected_segment(
    FA18ProjectedSegmentPreparationState *state,
    const FA18ViewVertex endpoints[2], FA18WorkspaceSegmentLineEmitter emitter,
    void *emitter_context) {
    FA18ViewVertex work[2];
    int16_t x[2], y[2];

    if (!state || !endpoints || !emitter) return -1;
    work[0] = endpoints[0];
    work[1] = endpoints[1];
    for (unsigned index = 0; index < 2; ++index) {
        if (!select_components(state, &work[0], &work[1])) return 0;
        if (!project_component(state->components.x, state->components.depth,
                               160, 160, 319, &x[index]) ||
            !project_component(state->components.y, state->components.depth,
                               90, 90, 179, &y[index]))
            return 0;
        if (!index) {
            const FA18ViewVertex swap = work[0];
            work[0] = work[1];
            work[1] = swap;
        }
    }
    return emitter(emitter_context, x[0], y[0], x[1], y[1]) == 0 ? 1 : -1;
}
