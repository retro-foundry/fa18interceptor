#include "menu.h"

#include "menu_frame200_data.h"
#include "menu_frame234_data.h"
#include "menu_frame236_data.h"

#include <string.h>

static void render_menu_runs(FA18IndexedFrameBuffer *framebuffer,
                             uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT],
                             const uint32_t *runs, size_t run_count) {
    memset(framebuffer->pixels, 0, sizeof framebuffer->pixels);
    for (size_t i = 0; i < run_count; ++i) {
        const uint32_t packed = runs[i];
        const uint16_t y = (uint16_t)(packed >> 24);
        const uint16_t start = (uint16_t)((packed >> 15) & 0x1ffu);
        const uint16_t length = (uint16_t)((packed >> 6) & 0x1ffu);
        const uint8_t index = (uint8_t)(packed & 0x3fu);
        for (uint16_t x = 0; x < length; ++x) {
            framebuffer->pixels[(size_t)y * FA18_WIDTH + start + x] = index;
        }
    }
    for (size_t i = 0; i < FA18_WIDTH * FA18_HEIGHT; ++i) {
        rgb444[i] = fa18_menu_frame200_palette[framebuffer->pixels[i]];
    }
}

void fa18_render_run075_frame200_menu(FA18IndexedFrameBuffer *framebuffer,
                                      uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT]) {
    if (!framebuffer || !rgb444) return;
    render_menu_runs(framebuffer, rgb444, fa18_menu_frame200_runs,
                     FA18_MENU_FRAME200_RUNS);
}

void fa18_render_run075_frame234_menu(FA18IndexedFrameBuffer *framebuffer,
                                      uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT]) {
    if (!framebuffer || !rgb444) return;
    render_menu_runs(framebuffer, rgb444, fa18_menu_frame234_runs,
                     FA18_MENU_FRAME234_RUNS);
}

void fa18_render_run075_frame235_clear(FA18IndexedFrameBuffer *framebuffer,
                                       uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT]) {
    if (!framebuffer || !rgb444) return;
    memset(framebuffer->pixels, 0, sizeof framebuffer->pixels);
    memset(rgb444, 0, sizeof(uint16_t) * FA18_WIDTH * FA18_HEIGHT);
}

void fa18_render_run075_frame236_demo_label(FA18IndexedFrameBuffer *framebuffer,
                                            uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT]) {
    if (!framebuffer || !rgb444) return;
    render_menu_runs(framebuffer, rgb444, fa18_menu_frame236_runs,
                     FA18_MENU_FRAME236_RUNS);
}

int fa18_select_run075_demo_mode(FA18MenuState *state) {
    /* $C1BD78-$C1BDEC, run075 direct-core frame 230: after the front-end
     * dispatch has supplied the first menu command, zero context mode and an
     * enabled record select the $7F demo mode. $C3318E is not included here.
     */
    if (!state || state->selected_mode != 0u) return -1;
    state->selection_marker = 1u;
    state->selected_mode = 0x7fu;
    return 0;
}

int fa18_schedule_demo_selection(FA18MenuState *state) {
    /* Original: source_amiga/observed/dispatch_top_level_menu_selection.asm,
     * $C0FD10-$C0FDCE. run075 enters $C0FD10 at frame 234 after $C11312
     * and $C2FD22; the rest of those helpers is outside this contract.
     */
    if (!state || state->selected_mode != 0x7fu) return -1;
    state->selector_queue[0] = 101u;
    state->selector_queue[1] = 0u;
    state->delay_ticks = (!state->video_auxiliary && (state->video_flags & 0x80u))
        ? 0x00d2u : 0x0096u;
    state->callback = FA18_MENU_CALLBACK_DELAYED_TRANSITION;
    return 0;
}

int fa18_menu_delay_expired(const FA18MenuState *state) {
    return state && state->delay_ticks < 0;
}

void fa18_menu_post_input_tick(FA18MenuState *state) {
    if (!state) return;
    ++state->post_input_tick_count;
    /* 68000 SUBQ.W wraps at 16 bits. Avoid signed-overflow undefined behavior
     * while preserving the source representation in a native signed field. */
    if (state->delay_ticks == INT16_MIN) state->delay_ticks = INT16_MAX;
    else --state->delay_ticks;
}

int fa18_prepare_run075_demo_transition(FA18MenuState *state) {
    /* $C0FEEA-$C0FFAC, bounded run075 continuation. $C11312, $C28722 and
     * $C11B0E have separate contracts, so their wider effects are excluded. */
    if (!state || state->selected_mode != 0x7fu || !fa18_menu_delay_expired(state)) {
        return -1;
    }
    state->transition_row_limit = 179;
    state->delay_ticks = 4;
    state->transition_stage = 3;
    state->post_input_phase = 2;
    state->demo_followup_pending = 0;
    state->transition_auxiliary = 1;
    state->callback = FA18_MENU_CALLBACK_DEMO_SETUP;
    return 0;
}

int fa18_enter_demo_followup(FA18MenuState *state) {
    /* Original $C1000A-$C10020: the $C0FFDA jump table selects this arm
     * for mode $7F. run075 hits it at frame 271 after the common setup.
     */
    if (!state || state->selected_mode != 0x7fu) return -1;
    state->demo_followup_pending = 1u;
    state->callback = FA18_MENU_CALLBACK_DEMO_ENTRY;
    return 0;
}

static void apply_demo_entry_followup(FA18MenuState *state) {
    state->followup_command_mode = 3;
    state->followup_auxiliary = 0;
    state->delay_ticks = 2;
    state->followup_mode = 0x0f;
    state->followup_input = 0;
    state->callback = FA18_MENU_CALLBACK_DEMO_FOLLOWUP_MATCH;
}

int fa18_finish_demo_entry_wait(FA18MenuState *state) {
    if (!state || state->callback != FA18_MENU_CALLBACK_DEMO_ENTRY ||
        !fa18_menu_delay_expired(state)) return -1;
    apply_demo_entry_followup(state);
    return 0;
}

int fa18_initialize_run075_demo_scene(FA18MenuState *state) {
    if (!state || state->selected_mode != 0x7fu || !fa18_menu_delay_expired(state)) {
        return -1;
    }
    /* $C0FAA4-$C0FB26, frame-370 run075 packet. The helpers at $C28722,
     * $C0924A, $C11312 and $C082B0 are intentionally not represented here. */
    state->scene_latch_previous = state->scene_latch_current;
    state->scene_latch_current = 0;
    state->scene_stage = 3;
    state->scene_guard = 1;
    state->selected_mode = 3;
    state->transition_auxiliary = 1;
    state->scene_counter_a = 0;
    state->scene_counter_b = 0;
    state->scene_marker = -1;
    state->delay_ticks = 1;
    return 0;
}

int fa18_expire_run075_demo_entry(FA18MenuState *state) {
    if (!state || state->callback != FA18_MENU_CALLBACK_DEMO_ENTRY ||
        !fa18_menu_delay_expired(state) ||
        fa18_initialize_run075_demo_scene(state) != 0) return -1;
    apply_demo_entry_followup(state);
    return 0;
}

int fa18_advance_demo_followup_match(FA18MenuState *state) {
    if (!state || state->callback != FA18_MENU_CALLBACK_DEMO_FOLLOWUP_MATCH) return -1;
    if (!fa18_menu_delay_expired(state)) return 0;
    state->transition_auxiliary = 0;
    if (state->followup_input != state->followup_expected_input) return 0;
    state->delay_ticks = 2;
    state->callback = FA18_MENU_CALLBACK_DEMO_FOLLOWUP_COMPLETE;
    return 1;
}

int fa18_complete_demo_followup(FA18MenuState *state) {
    if (!state || state->callback != FA18_MENU_CALLBACK_DEMO_FOLLOWUP_COMPLETE) {
        return -1;
    }
    if (!fa18_menu_delay_expired(state)) return 0;
    state->demo_followup_pending = 0;
    state->transition_auxiliary = 1;
    state->callback = FA18_MENU_CALLBACK_DEMO_CONTINUATION;
    return 1;
}
