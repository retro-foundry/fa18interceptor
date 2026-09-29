/* The game's own flight recorder. */
#include "flight_recorder.h"

#include "globals.h"
#include "memory.h"
#include "player_input.h"

enum { RECORDING = 0, PLAYING = 1, FULL = 4 };

void record_flight_input(void) {
    uint8_t mode;
    gaddr cursor, words;

    if (!rd_u8(RECORDER_ON) || !(rd_u16(COCKPIT_FLAGS) & 0x40)) return;
    mode = rd_u8(RECORDER_MODE);
    if (mode != RECORDING && mode != PLAYING) return;
    if (rd_u8(POST_INPUT_EVENT)) return;
    if (mode == PLAYING) {
        wr_u16(rd_u32(PLAYBACK_WORDS) + 2, rd_u16(RECORD_WORD_B));
        return;
    }
    cursor = rd_u32(RECORDER_CURSOR);
    if ((int32_t)(rd_u32(RECORDER_START) + rd_u32(RECORDER_SIZE)) <= (int32_t)cursor) {
        wr_u8(RECORDER_MODE, FULL);
        wr_u8(POST_INPUT_EVENT, 1);
        wr_u32(RECORDER_CURSOR, rd_u32(RECORDER_START));
        wr_u32(RECORDER_WORD_CURSOR, rd_u32(RECORDER_WORDS));
        return;
    }
    wr_u8(cursor, rd_u8(PLAYER_STICK));
    wr_u32(RECORDER_CURSOR, cursor + 1);
    words = rd_u32(RECORDER_WORD_CURSOR);
    wr_u16(words, rd_u16(RECORD_WORD_A));
    wr_u16(words + 2, rd_u16(RECORD_WORD_B));
    wr_u32(RECORDER_WORD_CURSOR, words + 4);
    wr_u16(RECORD_WORD_A, 0);
    wr_u16(RECORD_WORD_B, 0);
}

static int8_t ramp(int8_t old, uint8_t direction, uint8_t up, int step, int limit) {
    int value = old;
    if (!direction) return 0;
    if (direction == up) value = value < 0 ? step : value + step;
    else value = value > 0 ? -step : value - step;
    if (value > limit) value = limit;
    if (value < -limit) value = -limit;
    return (int8_t)value;
}

void update_flight_input(uint32_t player, uint32_t incoming_d0) {
    uint8_t mode = rd_u8(RECORDER_MODE);
    uint8_t stick;

    if (!rd_u16(CHOSEN_RECORD) && mode > 0 && mode <= 3) {
        gaddr bytes = rd_u32(PLAYBACK_BYTES);
        gaddr words = rd_u32(PLAYBACK_WORDS);
        uint32_t word_end;
        uint32_t last_d0;
        int ended;
        wr_u8(PLAYER_STICK, rd_u8(bytes++));
        wr_u32(PLAYBACK_BYTES, bytes);
        wr_u16(RECORD_WORD_A, rd_u16(words));
        if (mode != 1) wr_u16(RECORD_WORD_B, rd_u16(words + 2));
        words += 4;
        word_end = rd_u32(RECORDER_WORDS) + rd_u32(RECORDER_SIZE) * 4;
        if ((int32_t)word_end <= (int32_t)words) words = rd_u32(RECORDER_WORDS);
        wr_u32(PLAYBACK_WORDS, words);
        last_d0 = word_end;
        for (;;) {
            gaddr scan = bytes;
            ended = rd_u8(scan++) == 0xFF && rd_u8(scan++) == 0xFF && rd_u8(scan++) == 0xFF;
            if (ended || bytes == rd_u32(RECORDER_CURSOR)) break;
            last_d0 = rd_u32(RECORDER_START) + rd_u32(RECORDER_SIZE);
            if ((int32_t)last_d0 > (int32_t)bytes) break;
            bytes = rd_u32(RECORDER_START);
                wr_u32(PLAYBACK_BYTES, bytes);
            wr_u32(PLAYBACK_WORDS, rd_u32(RECORDER_WORDS) + 4);
        }
        if (ended || bytes == rd_u32(RECORDER_CURSOR)) {
            if (rd_u32(PLAYBACK_BYTES)) {
                wr_u32(PLAYBACK_BYTES, rd_u32(RECORDER_START));
                wr_u32(PLAYBACK_WORDS, rd_u32(RECORDER_WORDS) + 8);
                wr_u8(RECORDER_MODE, 3);
                wr_u8(0xC4582Au, 1);
            }
            incoming_d0 = last_d0;
            if (!rd_u8(KEY_TAKEN) && !(incoming_d0 & 0x80u)) {
                wr_u8(KEY_TAKEN, 1);
                if (rd_s8(KEY_COUNT) < 10) {
                    int8_t slot = rd_s8(KEY_WRITE);
                    if (slot >= 10) slot = 0;
                    wr_u8(KEY_RAW + (gaddr)(int32_t)slot, (uint8_t)incoming_d0);
                    wr_u8(KEY_WRITE, (uint8_t)(slot + 1));
                    wr_u8(KEY_COUNT, (uint8_t)(rd_u8(KEY_COUNT) + 1));
                    wr_u8(KEY_TRANSLATED + (gaddr)(int32_t)rd_s8(KEY_TRANSLATED_WRITE),
                          rd_u8(KEY_TABLE + (uint8_t)incoming_d0));
                }
            }
            wr_u8(KEY_STATE, 0);
            wr_u8(KEY_STATE + 1, 0);
            wr_u8(KEY_STATE + 2, 0);
            return;
        }
    }

    if (!rd_u16(CHOSEN_RECORD)) {
        if (rd_u8(player + 0x7C) & 0x0F) return;
        if (!rd_u8(POST_INPUT_EVENT)) {
            int8_t function = rd_s8(FUNCTION_KEY_LEVEL);
            if (function < 0) function = 0;
            if (function && mode != 3 && mode != 2) {
                uint8_t trim = rd_u8(player + 0x39) & 0x0F;
                int8_t difference = (int8_t)(function - rd_u8(player + 0x2B));
                if (difference < 0) {
                    if (difference < -8 || trim < 2) set_throttle_input(2);
                    else set_throttle_input(0);
                } else if (difference > 0) {
                    if (difference > 8 || trim < 2) set_throttle_input(1);
                    else set_throttle_input(0);
                } else if (rd_s8(player + 0x2B) < 0x78 ||
                           (rd_u8(player + 3) & 8) || (rd_u8(player + 2) & 0x20)) {
                    set_throttle_input(0);
                }
            }
        }
        if (mode == 0 || mode == 1) record_flight_input();
    }

    if (rd_u8(player + 4) & 0x10) {
        if (rd_s16(player + 0x4C) >= 0) {
            wr_u8(player + 0x28, 0);
            wr_u8(player + 0x29, 0);
            wr_u8(player + 0x2A, 0);
            return;
        }
        wr_u8(player + 4, rd_u8(player + 4) & (uint8_t)~0x10u);
    }
    stick = rd_u8(player + 0x65);
    wr_u8(player + 0x28, (uint8_t)ramp(rd_s8(player + 0x28), stick & 0x30, 0x10, 1, 20));
    wr_u8(player + 0x29, (uint8_t)ramp(rd_s8(player + 0x29), stick & 0xC0, 0x40, 1, 20));
    wr_u8(player + 0x2A, (uint8_t)ramp(rd_s8(player + 0x2A), stick & 0x0C, 0x04, 3, 60));
}
