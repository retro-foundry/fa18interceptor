/* Register effects of the two postflight scene callbacks ($C11788/$C11830). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "stages.h"
#include "glue_text.h"

int glue_C11830(void) {
    int8_t pose = rd_s8(SCENE_POSE_ENTRY);
    uint8_t context = rd_u8(CONTEXT_SELECT);

    D(0) = 0; /* MOVEQ, then the root's flag word with bit 1 set. */
    SET_W(D(0), rd_u16(CONTROL_RECORDS + 2) | 2u);
    SET_B(D(0), context);
    if (!context) view_mode_zero_registers();
    restart_postflight_scene();
    scene_setup_registers(pose);
    A(0) = 0xC11872u;
    return glue_return();
}

int glue_C11788(void) {
    uint8_t context = rd_u8(CONTEXT_SELECT);
    int8_t pose;
    int8_t remaining;

    SET_B(D(0), context);
    if (!context) {
        SET_B(D(0), rd_u8(PLAYER_FLAGS_A));
        if ((uint8_t)D(0)) {
            advance_postflight_reset();
            return glue_return();
        }
    } else {
        SET_W(D(0), rd_u16(CONTROL_RECORDS));
        if (D(0) & 0x0400u) {
            advance_postflight_reset();
            return glue_return();
        }
    }
    SET_W(D(0), rd_u16(COCKPIT_FLAGS) & 0x9FFFu);
    SET_W(D(0), (uint16_t)D(0) & 0xFFFEu);
    pose = rd_s8(SCENE_POSE_ENTRY);
    advance_postflight_reset();
    scene_setup_registers(pose);
    D(0) = 0; /* MOVEQ after scene setup. */
    SET_W(D(0), rd_u16(PLAYER_STATUS_D4));
    remaining = rd_s8(POSTFLIGHT_RESET_REMAINING);
    SET_B(D(0), (uint8_t)remaining);
    if (remaining > 0) {
        A(0) = 0xC11830u;
    } else {
        clear_render_buffers_registers();
        A(0) = 0xC118A0u;
    }
    return glue_return();
}
