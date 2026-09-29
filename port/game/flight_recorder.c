/* The game's own flight recorder. */
#include "flight_recorder.h"

#include "globals.h"
#include "memory.h"

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
