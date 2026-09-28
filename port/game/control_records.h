#ifndef FA18_GAME_CONTROL_RECORDS_H
#define FA18_GAME_CONTROL_RECORDS_H

#include <stdint.h>

#include "memory.h"

/* Control record fields used so far (meanings not yet assigned). */
enum {
    REC_FLAGS = 0x01,     /* byte: bit 6 = active */
    REC_FIELD_0C = 0x0C,  /* word */
    REC_FIELD_0E = 0x0E,  /* word */
    REC_FIELD_10 = 0x10,  /* long */
    REC_STATUS = 0x20,    /* byte: bit 1 = reportable */
    REC_PENDING = 0x50,   /* word: nonzero until settled */
    REC_PENDING_SIGN = 0x54 /* word: negated when settling */
};

/* The record whose index is the high byte of `selector`. */
gaddr control_record(uint16_t selector);

/* Read a record's +$0C, +$10 and +$0E fields (the words sign-extended). */
void read_record_fields(gaddr record, int32_t *field_0c, int32_t *field_10, int32_t *field_0e);

/* Drop the current selection unless its record is still active and not
 * reportable. */
void release_lost_selection(void);

/* Clear a record's pending word, negating +$54 if it was set. */
void settle_record(gaddr record);

/* Rate class 5, 3 or 1 from the largest of |+$56|, |+$58| and |+$5A|/4
 * (above $C0: 1; above $60: 3; else 5 unless |+$6C| > $1000). */
void classify_record_rate(gaddr record);

/* Clear the first (player) record's motion fields and the related globals. */
void reset_player_record(void);

/* Current record: +$5A = +$20 when +$6A exceeds 14400, -$20 when it is
 * lower but nonzero, 0 when +$6A is zero. */
void update_record_5a(void);

/* Current record: move +$26 an eighth of the way toward `target`. */
void ease_record_26(int16_t target);

/* Mark a record pending (+$50 = -1), negating +$54 if it was not already. */
void mark_record_pending(gaddr record);

/* Store view parameters into a record at +$2C..+$37. */
void set_record_view(gaddr record, int16_t a, int16_t b, int16_t c, int16_t d, uint32_t e);

#endif
