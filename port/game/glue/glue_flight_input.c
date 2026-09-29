/* Register replay for recorder playback and the three input ramps. */
#include "glue.h"
#include "ports_glue.h"

#include "flight_recorder.h"
#include "globals.h"
#include "memory.h"

static void ramp_register(uint8_t old, uint8_t direction, uint8_t positive,
                          int step, int limit) {
    if (!direction) return;
    SET_B(D(4), old);
    if (direction == positive) {
        if ((int8_t)old < 0) D(4) = (uint32_t)step;
        else SET_B(D(4), old + step);
        if ((int8_t)D(4) > limit) D(4) = (uint32_t)limit;
    } else {
        if ((int8_t)old > 0) D(4) = (uint32_t)(-step);
        else SET_B(D(4), old - step);
        if ((int8_t)D(4) < -limit) D(4) = (uint32_t)(-limit);
    }
}

static void recorder_call_registers(void) {
    if (!rd_u8(RECORDER_ON)) return;
    SET_W(D(0), rd_u16(COCKPIT_FLAGS) & 0x40);
    if (!(uint16_t)D(0)) return;
    SET_B(D(1), rd_u8(RECORDER_MODE));
    if (((uint8_t)D(1) != 0 && (uint8_t)D(1) != 1) || rd_u8(POST_INPUT_EVENT)) return;
    if ((uint8_t)D(1) == 1) A(0) = A(2);
    else {
        D(0) = rd_u32(RECORDER_START) + rd_u32(RECORDER_SIZE);
        A(0) = rd_u32(RECORDER_CURSOR);
        if ((int32_t)D(0) > (int32_t)A(0)) A(0)++;
    }
}

static int playback_registers(void) {
    uint8_t mode = rd_u8(RECORDER_MODE);
    uint32_t word_end;
    if ((int8_t)mode <= 0 || (int8_t)mode > 3) return 0;
    A(0) = rd_u32(PLAYBACK_BYTES);
    SET_B(D(0), rd_u8(A(0)));
    A(0)++;
    A(2) = rd_u32(PLAYBACK_WORDS);
    A(2) += 2;
    SET_W(D(0), rd_u16(A(2)));
    A(2) += 2;
    word_end = rd_u32(RECORDER_WORDS) + rd_u32(RECORDER_SIZE) * 4;
    D(0) = word_end;
    if ((int32_t)D(0) <= (int32_t)A(2)) A(2) = rd_u32(RECORDER_WORDS);
    for (;;) {
        A(3) = A(0);
        if (rd_u8(A(3)++) == 0xFF && rd_u8(A(3)++) == 0xFF &&
            rd_u8(A(3)++) == 0xFF) break;
        if (A(0) == rd_u32(RECORDER_CURSOR)) break;
        D(0) = rd_u32(RECORDER_START) + rd_u32(RECORDER_SIZE);
        if ((int32_t)D(0) > (int32_t)A(0)) return 0;
        A(0) = rd_u32(RECORDER_START);
        A(2) = rd_u32(RECORDER_WORDS) + 4;
    }
    if (!rd_u8(KEY_TAKEN) && !(D(0) & 0x80u) && rd_s8(KEY_COUNT) < 10) {
        SET_B(D(4), rd_u8(KEY_WRITE));
        if ((int8_t)D(4) >= 10) D(4) = 0;
        SET_W(D(4), (uint16_t)(int16_t)(int8_t)D(4));
        A(3) = KEY_RAW;
        SET_W(D(0), (uint16_t)D(0) & 0xFFu);
        A(3) = KEY_TABLE;
        SET_B(D(0), rd_u8(A(3) + (uint16_t)D(0)));
        SET_B(D(4), (uint8_t)(D(4) + 1));
        A(3) = KEY_TRANSLATED;
        SET_B(D(4), rd_u8(KEY_TRANSLATED_WRITE));
        SET_W(D(4), (uint16_t)(int16_t)(int8_t)D(4));
    }
    return 1;
}

static void flight_input_registers(gaddr player) {
    uint8_t mode = rd_u8(RECORDER_MODE), stick;
    uint8_t played_stick = ((int8_t)mode > 0 && (int8_t)mode <= 3)
                         ? rd_u8(rd_u32(PLAYBACK_BYTES)) : rd_u8(PLAYER_STICK);
    if (!rd_u16(CHOSEN_RECORD)) {
        SET_B(D(0), mode);
        if (playback_registers()) return;
        SET_B(D(0), rd_u8(player + 0x7C));
        SET_B(D(0), (uint8_t)D(0) & 0x0F);
        if ((uint8_t)D(0)) return;
        if (!rd_u8(POST_INPUT_EVENT)) {
            SET_B(D(0), rd_u8(FUNCTION_KEY_LEVEL));
            if ((uint8_t)D(0)) {
                if ((int8_t)D(0) < 0) SET_B(D(0), 0);
                SET_B(D(1), mode);
                if (mode != 3) {
                    SET_B(D(1), (uint8_t)(D(1) - 1));
                    if ((uint8_t)D(1)) {
                        int8_t diff;
                        uint8_t trim;
                        SET_B(D(1), rd_u8(player + 0x39));
                        SET_B(D(1), (uint8_t)D(1) & 0x0F);
                        trim = (uint8_t)D(1);
                        SET_B(D(0), (uint8_t)(D(0) - rd_u8(player + 0x2B)));
                        diff = (int8_t)D(0);
                        if (diff < 0) {
                            D(2) = diff < -8 || trim < 2 ? 2 : 0;
                            SET_B(D(1), (played_stick & 0xFC) | (uint8_t)D(2));
                        } else if (diff > 0) {
                            D(2) = diff > 8 || trim < 2 ? 1 : 0;
                            SET_B(D(1), (played_stick & 0xFC) | (uint8_t)D(2));
                        } else if (rd_s8(player + 0x2B) < 0x78 ||
                                   (rd_u8(player + 3) & 8) || (rd_u8(player + 2) & 0x20)) {
                            D(2) = 0;
                            SET_B(D(1), played_stick & 0xFC);
                        }
                    }
                }
            }
        }
        SET_B(D(4), mode);
        if (mode == 0 || mode == 1) recorder_call_registers();
    }
    SET_B(D(4), 0);
    if (rd_u8(player + 4) & 0x10) {
        if (rd_s16(player + 0x4C) >= 0) return;
    }
    stick = player + 0x65 == PLAYER_STICK && !rd_u16(CHOSEN_RECORD) &&
            (int8_t)mode > 0 && (int8_t)mode <= 3
            ? played_stick : rd_u8(player + 0x65);
    SET_B(D(2), stick);
    SET_B(D(3), (uint8_t)D(2));
    SET_B(D(2), (uint8_t)D(2) & 0x30);
    ramp_register(rd_u8(player + 0x28), (uint8_t)D(2), 0x10, 1, 20);
    SET_B(D(4), 0);
    SET_B(D(2), (uint8_t)D(3));
    SET_B(D(3), (uint8_t)D(3) & 0xC0);
    ramp_register(rd_u8(player + 0x29), (uint8_t)D(3), 0x40, 1, 20);
    SET_B(D(4), 0);
    SET_B(D(2), (uint8_t)D(2) & 0x0C);
    ramp_register(rd_u8(player + 0x2A), (uint8_t)D(2), 0x04, 3, 60);
}

int glue_C1B27E(void) {
    uint32_t incoming_d0 = D(0);
    gaddr player = A(1);
    flight_input_registers(player);
    update_flight_input(player, incoming_d0);
    return glue_return();
}
