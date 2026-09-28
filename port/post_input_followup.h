#ifndef FA18_POST_INPUT_FOLLOWUP_H
#define FA18_POST_INPUT_FOLLOWUP_H

#include <stdint.h>

#include "viewport_mode.h"

typedef enum {
    FA18_POST_INPUT_CALLBACK_FINISH_FOLLOWUP,
    FA18_POST_INPUT_CALLBACK_AFTER_FINISH_FOLLOWUP,
    FA18_POST_INPUT_CALLBACK_COMPLETE_FOLLOWUP,
    /* `$C0FA80` installs `$C10C08`; its subsequent controller is a separate
     * source owner, so retain this installed target without advancing it. */
    FA18_POST_INPUT_CALLBACK_CONTINUE_AFTER_COMPLETE_FOLLOWUP
} FA18PostInputCallback;

/* Caller-owned direct state surrounding `$C0FA04`. */
typedef struct {
    uint8_t command_mode;
    uint8_t auxiliary;
    uint8_t match_auxiliary;
    int16_t countdown;
    FA18PostInputCallback callback;
} FA18PostInputFollowupState;

typedef int (*FA18SceneInitializer)(void *context);

/* Execute the negative-countdown branch of `$C0FA04`. The supplied scene
 * initializer stands for the preceding `$C0FAA4` call and runs before every
 * direct followup store. A non-negative countdown belongs to `$C2FD22`, whose
 * behavior is deliberately outside this direct-store primitive. */
int fa18_finish_post_input_followup(FA18PostInputFollowupState *followup,
                                    FA18ViewportModeState *viewport_mode,
                                    FA18SceneInitializer scene_initializer,
                                    void *context);

/* `$C0FA4C`: wait for the viewport mode transition to reach its target after
 * the caller's countdown has expired, then arm the distinct `$C0FA80` stage. */
int fa18_advance_post_input_followup_match(FA18PostInputFollowupState *followup,
                                           const FA18ViewportModeState *viewport_mode);

/* `$C0FA80`: after its countdown expires, clear the caller-owned event flag,
 * mark the followup auxiliary byte, and install the `$C10C08` continuation. */
int fa18_complete_post_input_followup(FA18PostInputFollowupState *followup,
                                      uint8_t *event_flag);

#endif
