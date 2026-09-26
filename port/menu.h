#ifndef FA18_MENU_H
#define FA18_MENU_H

#include <stdint.h>

#include "renderer.h"

typedef enum {
    FA18_MENU_CALLBACK_SELECTION_FOLLOWUP,
    FA18_MENU_CALLBACK_DELAYED_TRANSITION,
    FA18_MENU_CALLBACK_DEMO_SETUP,
    FA18_MENU_CALLBACK_DEMO_ENTRY,
    FA18_MENU_CALLBACK_DEMO_FOLLOWUP_MATCH,
    FA18_MENU_CALLBACK_DEMO_FOLLOWUP_COMPLETE,
    FA18_MENU_CALLBACK_DEMO_CONTINUATION,
    FA18_MENU_CALLBACK_UNRESOLVED
} FA18MenuCallback;

/* Native menu state touched by the proved demo-selection path. The original
 * message-sequence head has already been reset before this contract begins.
 * Address references belong in the evidence comments and routine reports;
 * callbacks are represented here by their C-level meaning.
 */
typedef struct {
    uint8_t selected_mode;
    uint8_t selection_marker;
    uint8_t video_auxiliary;
    uint8_t video_flags;
    uint16_t selector_queue[2];
    int16_t delay_ticks;
    uint8_t post_input_tick_count;
    uint8_t transition_stage;
    uint8_t post_input_phase;
    uint8_t transition_auxiliary;
    int16_t transition_row_limit;
    uint8_t demo_followup_pending;
    uint8_t followup_command_mode;
    uint8_t followup_auxiliary;
    uint8_t followup_mode;
    uint8_t followup_input;
    uint8_t followup_expected_input;
    uint8_t scene_latch_current;
    uint8_t scene_latch_previous;
    uint8_t scene_stage;
    uint8_t scene_guard;
    uint16_t scene_counter_a;
    uint16_t scene_counter_b;
    int8_t scene_marker;
    FA18MenuCallback callback;
} FA18MenuState;

/* Exact run075 frame-200 menu page reconstructed as native indexed pixels.
 * The fixture is sourced from the verified display page, not Amiga address
 * state; it is the first native visual gate for the menu runtime. */
void fa18_render_run075_frame200_menu(FA18IndexedFrameBuffer *framebuffer,
                                      uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT]);

/* $C1BD78's proved run075 branch after the frontend input dispatcher has
 * recognized the recorded first menu command. The raw key-to-command mapping
 * and downstream $C3318E packet remain separate contracts. */
int fa18_select_run075_demo_mode(FA18MenuState *state);

/* Returns zero only for the proved demo path; no unproved mode is simulated. */
int fa18_schedule_demo_selection(FA18MenuState *state);

/* $C0FECE-$C0FEE9 reads this signed delay. A nonnegative value returns from
 * the callback; a negative value enters the mode-specific transition body. */
int fa18_menu_delay_expired(const FA18MenuState *state);

/* Proved state-update portion of $C0F5F8-$C0F811. The original invokes its
 * callback after this update; that callback dispatcher is a separate port
 * boundary. */
void fa18_menu_post_input_tick(FA18MenuState *state);

/* Observed run075 subset of the negative-delay common path before the $C0FFDA
 * mode table. Helper-call effects and unrelated transition fields remain out
 * of scope. */
int fa18_prepare_run075_demo_transition(FA18MenuState *state);

/* Original $C1000A-$C10020 branch, after the common transition setup. */
int fa18_enter_demo_followup(FA18MenuState *state);

/* $C0FA04's proved negative-delay state writes. Effects of its $C0FAA4 helper
 * and its nonnegative rendering call are separate contracts. */
int fa18_finish_demo_entry_wait(FA18MenuState *state);

/* Direct state subset of run075's $C0FAA4 scene initializer. Effects of its
 * three nested helpers remain separate port contracts. */
int fa18_initialize_run075_demo_scene(FA18MenuState *state);

/* Observed $C0FA04 expired route: initialize the run075 scene subset, then
 * apply the caller's direct followup writes that replace its delay of one. */
int fa18_expire_run075_demo_entry(FA18MenuState *state);

/* Byte-exact state gates for $C0FA4C and $C0FA80. They are reusable callback
 * contracts; their run075 use is not yet established. */
int fa18_advance_demo_followup_match(FA18MenuState *state);
int fa18_complete_demo_followup(FA18MenuState *state);

#endif
