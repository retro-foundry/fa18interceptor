#include "selected_display_submission.h"

int fa18_submit_selected_display_record_list(
    const FA18DisplayRecordSelectorOutput *selection,
    const FA18ProjectionPairSubmission *submission,
    FA18ProjectionPairBoundsRoute *bounds_route,
    FA18ProjectionPairFinalizationRoute *finalization_route) {
    FA18ProjectionPairScreenPoint points[6];
    FA18ProjectionPairBounds bounds;
    uint16_t count;

    if (!selection || !submission || !bounds_route || !finalization_route ||
        !selection->selection_flag)
        return -1;
    count = (uint16_t)selection->words[0];
    if (count < 3 || count > 6) return -1;
    for (uint16_t index = 0; index < count; ++index) {
        points[index].x = selection->words[1u + index * 2u];
        points[index].y = selection->words[2u + index * 2u];
    }
    if (fa18_reduce_projection_pair_bounds(points, count, &bounds) != 0 ||
        fa18_select_projection_pair_bounds_route(&bounds,
                                                 submission->display_bound_y,
                                                 bounds_route) != 0)
        return -1;
    if (*bounds_route == FA18_PROJECTION_PAIR_BOUNDS_BLITTER_LINE_C2FA7E) {
        *finalization_route = FA18_PROJECTION_PAIR_FINALIZATION_C3029E_FALLBACK;
        return fa18_submit_projection_pair_bounds(&bounds, submission->display_bound_y,
                                                  submission->line_emitter,
                                                  submission->line_context, bounds_route);
    }
    if (*bounds_route != FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION &&
        *bounds_route != FA18_PROJECTION_PAIR_BOUNDS_C302EC_CONTINUATION) {
        *finalization_route = FA18_PROJECTION_PAIR_FINALIZATION_C3029E_FALLBACK;
        return 0;
    }
    return fa18_submit_projection_pair_far_list(
        points, count, submission->display_bound_y, submission->vertical_value,
        submission->horizontal_value, submission->renderer_base_long,
        submission->mode_flag, submission->saved_line_scratch,
        submission->protected_line_emitter, submission->protected_line_context,
        submission->blitter_emitter, submission->final_emitter,
        submission->blitter_context, bounds_route, finalization_route);
}
