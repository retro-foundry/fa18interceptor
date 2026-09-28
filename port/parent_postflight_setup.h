#ifndef FA18_PARENT_POSTFLIGHT_SETUP_H
#define FA18_PARENT_POSTFLIGHT_SETUP_H

#include <stdint.h>

typedef int (*FA18ParentPostflightStage)(void *context);

typedef struct {
    FA18ParentPostflightStage prepare;
    FA18ParentPostflightStage secondary_prepare;
    FA18ParentPostflightStage helper[9];
    void *context;
} FA18ParentPostflightSetupOps;

typedef struct {
    uint8_t postflight_flag;
    uint8_t mode_flag;
    uint8_t selected_record_type;
    uint8_t activity_byte;
    uint16_t local_frame;
    uint16_t stage_marker;
} FA18ParentPostflightSetupState;

typedef enum {
    FA18_PARENT_POSTFLIGHT_CONTINUE_ACTIVITY,
    FA18_PARENT_POSTFLIGHT_SKIP_TO_TAIL,
    FA18_PARENT_POSTFLIGHT_SKIP_TO_TAIL_CODE
} FA18ParentPostflightSetupRoute;

/* `$C0F124-$C0F1E1`: record selection and activity-gated helper prefix.
 * `selected_record_type` is caller-resolved from `$C46184 + ($C458DC << 9)`.
 */
int fa18_run_parent_postflight_setup(FA18ParentPostflightSetupState *state,
                                     const FA18ParentPostflightSetupOps *ops,
                                     FA18ParentPostflightSetupRoute *route);

#endif
