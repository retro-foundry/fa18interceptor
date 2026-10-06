/* Small update stages. */
#include "stages.h"
#include "menu_context_finish.h"
#include "postflight_completion.h"
#include "command_publication.h"

#include "cockpit.h"
#include "audio.h"
#include "fault.h"
#include "fixed_math.h"
#include "globals.h"
#include "player_input.h"
#include "render_buffers.h"
#include "scene_setup.h"
#include "scene_dispatch.h"
#include "target_heading.h"
#include "view.h"

void initialize_scene_state(const SceneStartHooks *hooks) {
    wr_u8(SCENE_DISPATCH_LIMIT_PREVIOUS, rd_u8(SCENE_DISPATCH_LIMIT));
    wr_u8(SCENE_DISPATCH_LIMIT, 0);
    wr_u8(SCENE_POSE_ENTRY, 4);
    wr_u8(SCENE_POSE_ENTRY, 3);
    wr_u8(CONTEXT_STATE, 0);
    wr_u8(PAUSE_A, 0);
    wr_u8(RECORDER_ON, 1);
    if (hooks && hooks->select_scene) hooks->select_scene(hooks->context);
    else initialize_scene_from_mode(0);
    wr_u8(MODE_SELECT, 3);
    if (hooks && hooks->reset_root) hooks->reset_root(hooks->context);
    else reset_scene_context();
    if (hooks && hooks->reset_messages) hooks->reset_messages(hooks->context);
    else reset_message_sequence();
    wr_u8(CONTEXT_GATE, 0);
    wr_u8(POST_INPUT_AUX, 1);
    wr_u16(SPAN_ORIGIN, 0);
    wr_u16(SPAN_ORIGIN_Y, 0);
    if (hooks && hooks->finish) hooks->finish(hooks->context);
    else finish_scene_setup();
    wr_u8(UPDATE_MASK, 0xFF);
    wr_u16(POST_INPUT_COUNTDOWN, 1);
}

void finish_post_input_followup(const PostInputFollowupHooks *hooks) {
    if (rd_s16(POST_INPUT_COUNTDOWN) >= 0) {
        if (hooks && hooks->clear_buffers) hooks->clear_buffers(hooks->context);
        else clear_render_buffers();
        return;
    }
    if (hooks && hooks->start_scene) hooks->start_scene(hooks->context);
    else initialize_scene_state(0);
    wr_u8(RECORDER_MODE, 3);
    wr_u8(CONTEXT_SELECT, 0);
    wr_u16(POST_INPUT_COUNTDOWN, 2);
    wr_u8(VIEWPORT_TARGET, 0x0F);
    wr_u8(VIEWPORT_MODE, 0);
    wr_u32(STAGE_CALLBACK, ROUTINE_POST_INPUT_MATCH);
}

void queue_post_input_context_command(void) {
    queue_menu_context_command(0,NULL);
}

void empty_stage(void) {}

void reset_list(void) {
    wr_u16(LIST_COUNT, 0);
    wr_u32(LIST_WRITE, LIST_BUFFER);
}

void start_view_mode_zero(uint8_t raw_key) {
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    wr_u8(0xC457A8u, 0);
    wr_u8(VIEW_MODE, 0);
    wr_u8(REDRAW_FIRST, 3);
    set_zoom_maximum();
    wr_u16(0xC45936u, 0xFFFF);
    wr_u8(UPDATE_MASK, 0xFF);
    wr_u8(0xC457A9u, 0);
    wr_u8(0xC45891u, 0xFF);
    if ((rd_u8(record + 0x62) & 0xF0) != 0x30) {
        int16_t origin = rd_s8(0xC1BAD4u);
        wr_s16(SPAN_ORIGIN, origin);
        wr_s16(SPAN_ORIGIN_Y, (int16_t)(origin << 4));
        request_cockpit_redraw();
        wr_u16(LINE_LAST_ROW, 0x90);
    }
    publish_command_event(raw_key,NULL);
}

void update_view_controls(void) {
    gaddr record;
    uint8_t type;
    uint16_t command;

    if (!rd_u8(CONTEXT_SELECT) && !rd_u8(0xC45891u))
        start_view_mode_zero(0);
    record = CONTROL_RECORDS + (gaddr)((int32_t)rd_s16(TARGET_RECORD) << 9);
    type = rd_u8(record + 0x62) & 0xF0;
    command = rd_u16(COMMAND_WORD);
    if (command & 2) {
        wr_u16(COMMAND_WORD, command & (uint16_t)~2u);
        wr_u8(FIRE_STATE, 0xFE);
        if (rd_u8(CONTEXT_SELECT)) {
            wr_u8(CONTEXT_SELECT, 0);
            wr_u16(SPAN_ORIGIN, 0);
            wr_u8(TRACK_STARTED, 0);
            wr_u8(0xC45835u, 1);
        } else {
            wr_u8(CONTEXT_SELECT, rd_u8(0xC45833u));
            wr_u16(SPAN_ORIGIN, 0x32);
        }
        free_voice(3);
        if (type == 0x30) wr_u16(SPAN_ORIGIN, 0x32);
        wr_u16(SPAN_ORIGIN_Y, (uint16_t)(rd_u16(SPAN_ORIGIN) << 4));
        set_zoom_maximum();
        wr_u8(VIEW_MODE, 0);
        wr_u8(UPDATE_MASK, 0xFF);
        queue_view_key((uint8_t)rd_u16(SPAN_ORIGIN_Y));
        if (type == 0x30 || rd_u8(CONTEXT_SELECT)) {
            wr_u16(LINE_LAST_ROW, rd_u8(PAUSE_A) ? 0xB3 : 0xA7);
        }
    }
    if (type == 0x30 && rd_u16(SPAN_ORIGIN) == 0) {
        wr_u16(SPAN_ORIGIN, 0x32);
        wr_u16(SPAN_ORIGIN_Y, 0x320);
        set_zoom_maximum();
        wr_u8(VIEW_MODE, 0);
        wr_u8(UPDATE_MASK, 0xFF);
        queue_view_key(0);
        wr_u16(LINE_LAST_ROW, 0xA7);
    }
    if (!rd_u8(CONTEXT_SELECT)) return;
    if (rd_u16(COMMAND_WORD) & 0x10) {
        wr_u16(COMMAND_WORD, rd_u16(COMMAND_WORD) & (uint16_t)~0x10u);
        wr_u8(TARGET_ENABLED, rd_u8(TARGET_ENABLED) ^ 1);
    }
    if (rd_u8(CONTEXT_STATE) == 6 && rd_s32(0xC45C42u) < 0x24000 &&
        rd_u16(LINE_LAST_ROW) != 0xA7) {
        wr_u16(LINE_LAST_ROW, 0xA7);
        request_cockpit_redraw();
    }
}

void set_event_bit_and_clear_command_word_bit(void) {
    /* $C08394: BSET.B #3 and ANDI.W #$FFF7. */
    wr_u8(EVENT_FLAG_BYTE, (uint8_t)(rd_u8(EVENT_FLAG_BYTE) | 8));
    wr_u16(COMMAND_WORD, (uint16_t)(rd_u16(COMMAND_WORD) & 0xFFF7u));
}

void reset_throttle_input_state(void) {
    /* $C1B602: the three CLR stores in their original order. */
    wr_u8(FUNCTION_KEY_LEVEL, 0);
    wr_u16(CONTROL_ACCUMULATOR_Y, 0);
    wr_u16(CONTROL_ACCUMULATOR_COMPANION, 0);
}

uint8_t dispatch_space_command_effect(void) {
    uint8_t command = (uint8_t)(rd_u8(CONTROL_RECORDS + 0x7C) & 15);
    if (command) return command;
    if (rd_u8(MODE_SELECT) == 0x7D) {
        if (rd_s8(COMMAND_STATUS_BYTE) < 0)
            wr_u8(SECONDARY_REQUEST_FLAGS, (uint8_t)(rd_u8(SECONDARY_REQUEST_FLAGS) | 8));
        return command;
    }
    wr_u8(EVENT_FLAG_BYTE, (uint8_t)(rd_u8(EVENT_FLAG_BYTE) | 4));
    command = (uint8_t)(rd_u8(SPACE_COMMAND_MODE) & 0xF0);
    if (command == 0x10) wr_u16(COMMAND_WORD, (uint16_t)(rd_u16(COMMAND_WORD) | 8));
    else if (command) wr_u8(SPACE_COMMAND_LATCH, 1);
    return command;
}

void queue_postflight_failure_message(void) { queue_postflight_failure(NULL); }

void restart_postflight_scene(void) { restart_postflight_completion(NULL); }

void advance_postflight_reset(void) { advance_postflight_completion(NULL); }
