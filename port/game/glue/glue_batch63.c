/* Leaves whose observed callers do not read the changed condition codes or
 * scratch registers. See source_amiga/observed/ for the byte-exact bodies. */
#include "glue.h"
#include "ports_glue.h"

#include "stages.h"
#include "control_records.h"
#include "globals.h"
#include "memory.h"
#include "numbers.h"

int glue_C08394(void) {
    set_event_bit_and_clear_command_word_bit();
    return glue_return();
}

int glue_C090C2(void) {
    clear_scene_startup_state();
    return glue_return();
}

int glue_C090F2(void) {
    enable_scene_record_updates();
    return glue_return();
}

int glue_C1B602(void) {
    reset_throttle_input_state();
    return glue_return();
}

int glue_C0833E(void) {
    SET_B(D(4), dispatch_space_command_effect());
    return glue_return();
}

int glue_C133B2(void) {
    gaddr record = rd_u32(CURRENT_RECORD);
    int16_t value = rd_s16(record + 0x6E);
    A(0) = record;
    D(0) = (uint32_t)record_6e_step();
    if ((rd_u8(record + 0x62) & 0xF0) != 0x30) {
        if (value < 0) value = (int16_t)(0u - (uint16_t)value);
        SET_W(D(1), (uint16_t)(value >> 9));
    }
    return glue_return();
}

int glue_C083E2(void) {
    begin_mission_reset();
    D(0) = (D(0) & 0x0000FFFFu) | 0x40000000u;
    return glue_return();
}

int glue_C25A00(void) {
    uint16_t repeats = (uint16_t)D(6);
    uint32_t weight = rd_u32(A(5)), before = D(7);
    D(7) = add_repeated_nibble_weight(A(5), repeats, D(7));
    if (repeats) {
        before = D(7) - weight;
        FLAG_N = NFLAG_32(D(7));
        FLAG_Z = D(7);
        FLAG_V = VFLAG_ADD_32(weight, before, D(7));
        FLAG_C = CFLAG_ADD_32(weight, before, D(7));
        FLAG_X = FLAG_C ? XFLAG_SET : XFLAG_CLEAR;
    }
    SET_W(D(6), 0xFFFF);
    A(5) += 4;
    return glue_return();
}
