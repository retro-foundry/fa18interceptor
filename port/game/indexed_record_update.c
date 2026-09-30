/* Selected control-record setup and phase update ($C13D84-$C1414E).
 * The remainder of $C13D84 is not yet registered. */
#include "indexed_record_update.h"

#include "globals.h"
#include "control_records.h"

static uint8_t phase(gaddr record) { return rd_u8(record + 0x2B); }
static void set_phase(gaddr record, uint8_t value) { wr_u8(record + 0x2B, value); }
static uint16_t header(gaddr record) { return rd_u16(record + 2); }
static void set_header(gaddr record, uint16_t value) { wr_u16(record + 2, value); }

static void phase_rise(gaddr record, int16_t index, int16_t step) {
    int8_t value;
    set_header(record, header(record) & (uint16_t)~0x0400u);
    value = (int8_t)(phase(record) + step);
    set_phase(record, (uint8_t)value);
    if (value < 0x78) return;

    if (header(record) & 0x2000u) {
        if (!(header(record) & 0x0008u)) {
            set_header(record, header(record) | 0x0008u);
            wr_u8(BAR_REDRAWS_A, 2);
            if (index == rd_s16(TARGET_RECORD)) {
                wr_u8(FIRE_STATE, 0xFD);
                wr_u8(FIRE_ALERT_COUNTDOWN, 6);
            }
        }
    } else {
        set_header(record, header(record) | 0x2000u);
        wr_u8(record + 0x65, rd_u8(record + 0x65) & 0xFCu);
        if (index == 0) wr_u8(FUNCTION_KEY_LEVEL, 0);
    }
    set_phase(record, 0x78);
}

static void phase_fall(gaddr record, int16_t index, int16_t step) {
    int8_t value;
    uint16_t bits = header(record) & (uint16_t)~0x2000u;
    set_header(record, bits);
    if (bits & 0x0008u) {
        set_header(record, bits & (uint16_t)~0x0008u);
        set_phase(record, (uint8_t)(phase(record) - 1));
        wr_u8(BAR_REDRAWS_A, 2);
        if (index == rd_s16(TARGET_RECORD)) wr_u8(FIRE_STATE, 0xFC);
        return;
    }

    value = (int8_t)phase(record);
    if (value >= 0) {
        value = (int8_t)(value - 4);
        set_phase(record, (uint8_t)value);
        if (value >= 0) return;
        if (rd_u8(record + 4) & 0x08u) {
            set_phase(record, 0);
            return;
        }
        if (bits & 0x0400u) return;
        wr_u8(record + 0x65, rd_u8(record + 0x65) & 0xFCu);
        set_phase(record, 0);
        set_header(record, bits | 0x0400u);
        return;
    }

    value = (int8_t)(value - step);
    set_phase(record, (uint8_t)value);
    if (value < -0x20) set_phase(record, 0xE0);
}

static void publish_control_words(gaddr record, int16_t distance) {
    int16_t word = rd_s16(CONTROL_ACCUMULATOR_Y);
    if ((int8_t)rd_u8(MODE_SELECT) == 2 || !rd_u8(PLAYER_READY)) return;
    if (word < 0x3C0 && distance <= 0x10) return;

    if (word < 0) {
        wr_u16(CONTROL_ACCUMULATOR_Y, 0);
        wr_u8(FUNCTION_KEY_LEVEL, 0xFF);
    } else if (header(record) & 0x0008u) {
        if (word < 0x360) wr_u8(FUNCTION_KEY_LEVEL, (uint8_t)(word >> 3));
    } else if (word != 0) {
        if (!phase(record) && !rd_u16(TARGET_RECORD)) wr_u8(FIRE_STATE, 0xFE);
        wr_u8(FUNCTION_KEY_LEVEL, (uint8_t)(word >> 3));
    }
    wr_u16(CONTROL_ACCUMULATOR_COMPANION, rd_u16(CONTROL_ACCUMULATOR_Y));
}

void prepare_indexed_record_context(IndexedRecordWork *work) {
    gaddr record;
    int16_t index = rd_s16(STREAM_MODE);
    int16_t distance = 0;
    int16_t step = 0;
    uint8_t controls;

    record = CONTROL_RECORDS + (gaddr)(int32_t)(index * CONTROL_RECORD_BYTES);
    work->record = record;
    work->index = index;
    work->neg_word_6c = (int16_t)-rd_s16(record + 0x6C);
    wr_u32(CURRENT_RECORD, record);

    if (!rd_u32(record + 0x72)) {
        set_phase(record, 0);
        if (index == 0 && rd_u8(PLAYER_READY)) {
            wr_u16(CONTROL_ACCUMULATOR_COMPANION, 0);
            wr_u16(CONTROL_ACCUMULATOR_Y, 0);
        }
        set_header(record, header(record) & (uint16_t)~0x0008u);
    } else {
        if (index == 0) {
            distance = (int16_t)(rd_u16(CONTROL_ACCUMULATOR_COMPANION) -
                                 rd_u16(CONTROL_ACCUMULATOR_Y));
            if (distance < 0) distance = (int16_t)-distance;
        }

        controls = rd_u8(record + 0x65) & 3u;
        if (controls == 0) {
            wr_u8(record + 0x39, (rd_u8(record + 0x39) & 0xF0u) | 1u);
        } else {
            uint8_t count = (uint8_t)((rd_u8(record + 0x39) & 0x0Fu) + 1u);
            if (count > 8) count = 8;
            else wr_u8(record + 0x39, (rd_u8(record + 0x39) & 0xF0u) | count);
            step = (int16_t)(count >> 1);
            if (step && !phase(record) && index == rd_s16(TARGET_RECORD))
                wr_u8(FIRE_STATE, 0xFE);

            if (controls == 1) phase_rise(record, index, step);
            else if (controls == 2) phase_fall(record, index, step);

            if (index == 0) {
                int16_t clipped = rd_s8(record + 0x2B);
                if (clipped < 0) clipped = 0;
                if (rd_u8(PLAYER_READY) &&
                    (rd_s16(CONTROL_ACCUMULATOR_Y) >= 0x3C0 || distance < 0x10)) {
                    uint16_t scaled = (uint16_t)(clipped << 3);
                    wr_u16(CONTROL_ACCUMULATOR_COMPANION, scaled);
                    if (rd_s16(CONTROL_ACCUMULATOR_Y) >= 0)
                        wr_u16(CONTROL_ACCUMULATOR_Y, scaled);
                }
            }
        }
        if (index == 0) publish_control_words(record, distance);
    }

    work->reference_distance = distance;
    work->phase_byte_scaled = (int16_t)((uint32_t)(int32_t)rd_s8(record + 0x2B) << 8);
}
