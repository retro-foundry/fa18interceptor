/* Selected control-record setup and phase update ($C13D84-$C1414E).
 * The remainder of $C13D84 is not yet registered. */
#include "indexed_record_update.h"

#include "globals.h"
#include "control_records.h"
#include "fixed_math.h"

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

IndexedSignedRoute prepare_indexed_signed_terms(IndexedRecordWork *work) {
    gaddr record = work->record;
    int16_t current = (int16_t)-work->neg_word_6c;
    int16_t bound;

    work->current_word_6c = current;
    work->attitude_adjustment = current < 0x800 ? 0 : attitude_term();
    if (!(rd_u16(COCKPIT_FLAGS) & 0x40u) || (rd_u16(MESSAGE_STATE) & 0x4000u))
        return INDEXED_COMMON_TAIL;
    if (!work->phase_byte_scaled && !(header(record) & 0x0008u))
        return INDEXED_EMPTY_ROUTE;
    if (!rd_u32(record + 0x72)) return INDEXED_EMPTY_ROUTE;
    if (!(header(record) & 0x0008u)) return INDEXED_OTHER_ROUTE;

    wr_u32(record + 0x72, rd_u32(record + 0x72) - 0x600u);
    bound = (int16_t)(0x3A98 - work->attitude_adjustment);
    if (bound > 0x4650) bound = 0x4650;
    if (!(rd_u8(record + 0x7C) & 0x70u)) bound = (int16_t)(bound - (bound >> 2));
    if (rd_u16(record) & 0x0800u) bound = (int16_t)(bound - (bound >> 2));
    work->comparison_bound = bound;

    if (current < bound) {
        if (work->neg_word_6c > 0) work->neg_word_6c = (int16_t)(work->neg_word_6c - 0x78);
        else if (!(header(record) & 0x0080u)) {
            work->neg_word_6c = (int16_t)(work->neg_word_6c -
                                        (current > 0x1D4C ? 0x2D : 0x36));
        } else {
            ease_record_26(-0x25);
            work->neg_word_6c = (int16_t)(work->neg_word_6c + rd_s16(record + 0x26));
        }
    } else if ((int32_t)current > (int32_t)bound + 0x3C) {
        work->neg_word_6c = (int16_t)(work->neg_word_6c + 0x3C);
    }
    return INDEXED_COMMON_TAIL;
}

void settle_empty_indexed_record(IndexedRecordWork *work) {
    gaddr record = work->record;
    int16_t value = work->neg_word_6c;
    int16_t current = work->current_word_6c;

    if (header(record) & 0x0080u) {
        ease_record_26(0);
        if (value > 0) value = value > 15 ? (int16_t)(value - 15) : 0;
        else value = current > 15 ? (int16_t)(value + 15) : 0;
    } else if (value > 0) {
        if (value > 0x90) value = value > 0x52 ? (int16_t)(value - 0x52) : 0;
        else value = value > 0x3C ? (int16_t)(value - 0x3C) : 0;
    } else {
        if (current > 0x1D4C) value = (int16_t)(value + 0x52);
        if (current < 0x5DC) {
            if (current > 0x1E) value = (int16_t)(value + 0x1E);
        } else {
            value = current > 0x3C ? (int16_t)(value + 0x3C) : 0;
        }
    }
    work->neg_word_6c = value;
}

void finish_indexed_record_update(IndexedRecordWork *work) {
    gaddr record = work->record;
    gaddr word_6c = record + 0x6C;
    uint16_t h = header(record);
    int16_t value = (int16_t)-work->neg_word_6c;
    int use_bounded_decay = 0;

    work->current_word_6c = value;
    if (work->index == 0) {
        uint16_t flags = rd_u16(0xC458D2u);
        if (value < 0x20D0) wr_u16(0xC458D2u, flags & (uint16_t)~0x4000u);
        else if (!(flags & 0x4000u)) {
            wr_u16(0xC458D2u, flags | 0x4000u);
            wr_u8(0xC457C0u, 3);
            wr_u8(FIRE_STATE, 0xFA);
            wr_u8(FIRE_ALERT_COUNTDOWN, 2);
        }
    }

    if ((h & 0xC000u) == 0xC000u) decay_outside_limit(word_6c, 7, 3);
    else if ((h & 0x0080u) && (rd_u16(record) & 0x0800u) && rd_s16(word_6c) > 0x40)
        decay_outside_limit(word_6c, 15, 4);
    else if ((h & 0x0080u) && (rd_u8(record + 0x7C) & 0x80u))
        decay_outside_limit(word_6c, 31, 5);
    else if (rd_u8(record + 0x20) & 1u) decay_outside_limit(word_6c, 3, 2);
    else if (rd_u8(record + 4) & 0x08u) use_bounded_decay = 1;
    else wr_u16(word_6c, (uint16_t)value);

    if (use_bounded_decay) {
        int32_t phase_value = rd_s8(record + 0x2B);
        if (phase_value < 0x6C && phase_value > -0x6C) decay_outside_limit(word_6c, 7, 3);
        else wr_u16(word_6c, (uint16_t)value);
    }
    update_record_76_78();
    record = rd_u32(CURRENT_RECORD);
    wr_u16(record + 0x6E, (uint16_t)(rd_u16(record + 0x78) + rd_u16(record + 0x6C)));
}
