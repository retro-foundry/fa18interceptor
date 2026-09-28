#ifndef FA18_SELECTED_DISPLAY_SUBMISSION_H
#define FA18_SELECTED_DISPLAY_SUBMISSION_H

#include "display_record_selector.h"
#include "projection_grid.h"

/* `$C2FEDE -> $C301F6`: submit the selected `$C4B390` point list directly,
 * without the separate `$C2FF48` DMA wrapper. */
int fa18_submit_selected_display_record_list(
    const FA18DisplayRecordSelectorOutput *selection,
    const FA18ProjectionPairSubmission *submission,
    FA18ProjectionPairBoundsRoute *bounds_route,
    FA18ProjectionPairFinalizationRoute *finalization_route);

#endif
