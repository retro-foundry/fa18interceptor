/* Indexed control-record phase, signed response, and damping ($C13D84). */
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

static int16_t toward_bound_without_flag(IndexedRecordWork *work, int16_t bound) {
    gaddr record = work->record;
    int16_t current = work->current_word_6c;
    int16_t neg = work->neg_word_6c;

    bound = (int16_t)(bound - (bound >> 2));
    if (rd_u8(record + 0x20) & 1u) bound = (int16_t)(bound >> 2);
    if (bound > 0x1FEF) bound = 0x1FEF;
    bound = (int16_t)(bound - work->attitude_adjustment);
    if (rd_u16(record) & 0x0800u) bound = (int16_t)(bound - (bound >> 2));
    if (!(rd_u8(record + 0x7C) & 0x70u)) bound = (int16_t)(bound - (bound >> 2));
    work->comparison_bound = bound;

    if (current < bound) {
        if (neg > 0) neg = (int16_t)(neg - 0x78);
        else neg = (int16_t)(neg - ((int32_t)bound - current < 0x150 ? 5 : 0x2A));
        if ((int16_t)-neg > bound) neg = (int16_t)-bound;
    } else if ((int32_t)current > (int32_t)bound + 0x3C) {
        if (current > 0x1D4C) neg = (int16_t)(neg + 0x52);
        else neg = (int16_t)(neg + ((int32_t)current - bound < 0x1E0 ? 7 : 0x3C));
    }
    return neg;
}

static int16_t toward_bound_with_flag(IndexedRecordWork *work, int16_t bound) {
    gaddr record = work->record;
    int16_t current = work->current_word_6c;
    int16_t delta = 0;
    uint8_t flags = rd_u8(record + 4);

    bound = (int16_t)(bound - (bound >> 2));
    if (!(flags & 0x08u) && rd_s8(record + 0x2B) < 0x0C) bound = 0;
    if (!(flags & 0x44u)) bound = (int16_t)(bound >> 1);
    else if (rd_u8(record + 0x20) & 1u) bound = 0;
    work->comparison_bound = bound;

    if (current < bound) {
        if (work->neg_word_6c > 0) delta = -0x78;
        else delta = (int16_t)-((int16_t)((int16_t)(bound - current) >> 8) +
                                 ((flags & 0x08u) ? 0x42 : 0x16));
    } else if ((int32_t)current > (int32_t)bound + 15) delta = 15;
    ease_record_26(delta);
    if (delta) work->neg_word_6c = (int16_t)(work->neg_word_6c + rd_s16(record + 0x26));
    return bound;
}

void update_indexed_other_route(IndexedRecordWork *work) {
    gaddr record = work->record;
    int16_t phase_value = work->phase_byte_scaled;
    int16_t magnitude = phase_value < 0 ? (int16_t)-phase_value : phase_value;
    int16_t reduction = (int16_t)(magnitude >> 9);
    uint16_t h = header(record);

    wr_u32(record + 0x72, rd_u32(record + 0x72) - (uint32_t)(int32_t)reduction);
    if (phase_value >= 0) {
        work->current_word_6c = (int16_t)-work->neg_word_6c;
        if (h & 0x0080u) {
            int16_t bound = (int16_t)(phase_value >> 3);
            toward_bound_with_flag(work, bound);
        } else {
            int16_t bound = (int16_t)(phase_value >> 1);
            bound = (int16_t)(bound - (phase_value >> 3));
            work->neg_word_6c = toward_bound_without_flag(work, bound);
        }
    } else if (!(h & 0x0080u)) {
        int16_t neg = work->neg_word_6c;
        if (neg < 0) neg = (int16_t)(neg + 0x57);
        else {
            int16_t bound = (int16_t)-phase_value;
            bound = (int16_t)(bound - (bound >> 2));
            if (neg > bound) neg = (int16_t)(neg - 0x2A);
            else if (neg < (int16_t)(bound - 0x2A)) neg = (int16_t)(neg + 0x2A);
        }
        work->neg_word_6c = neg;
    } else {
        int16_t delta = 0;
        if (work->neg_word_6c < 0) delta = 0x3C;
        else {
            int16_t bound = (int16_t)((int16_t)-phase_value >> 4);
            bound = (int16_t)(bound - (bound >> 2));
            if (!(rd_u8(record + 4) & 0x04u)) bound = (int16_t)(bound >> 1);
            if (work->neg_word_6c > bound) delta = -0x16;
            else if (work->neg_word_6c < (int16_t)(bound - 0x16)) delta = 0x16;
            work->comparison_bound = bound;
        }
        ease_record_26(delta);
        if (delta) work->neg_word_6c = (int16_t)(work->neg_word_6c + rd_s16(record + 0x26));
    }
}

void update_indexed_record(IndexedRecordWork *work) {
    IndexedSignedRoute route;
    prepare_indexed_record_context(work);
    route = prepare_indexed_signed_terms(work);
    if (route == INDEXED_EMPTY_ROUTE) settle_empty_indexed_record(work);
    else if (route == INDEXED_OTHER_ROUTE) update_indexed_other_route(work);
    finish_indexed_record_update(work);
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
    work->helper_record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
    work->helper_m6c = rd_s16(work->helper_record + 0x6C);
    work->helper_m6e = rd_s16(work->helper_record + 0x6E);
    work->helper_old76 = rd_s16(work->helper_record + 0x76);
    work->helper_enabled = rd_u8(RECORD_UPDATES_ON) != 0;
    update_record_76_78();
    record = rd_u32(CURRENT_RECORD);
    wr_u16(record + 0x6E, (uint16_t)(rd_u16(record + 0x78) + rd_u16(record + 0x6C)));
}
