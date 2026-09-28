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

/* Set bit 4 of byte +1 in every control record and workspace record. */
void flag_all_records(void);

/* The two display bytes of a record entry: its own +$0E word (high, low)
 * unless bit 4 of its first word is set, when they come from the control
 * record its high byte selects (+$06 and +$08 low bytes). */
void read_record_pair(gaddr entry, int16_t *high, int16_t *low);

/* Heading matrix of the viewed record into VIEW_MATRIX. */
void update_view_matrix(void);

/* Compass heading of the viewed record, in degrees and tape steps. */
void update_compass(void);

/* Nudge *value away from zero by the current record's +$6C / 128, except
 * inside the +-$500 dead zone. */
void nudge_outside_dead_zone(gaddr value);

/* Reset the player record's mission fields and clear records 1-3. */
void reset_mission_objects(void);

/* Set up the player record for a new flight. */
void prepare_player_record(void);

/* Ease the current record's +$56 a quarter of the way toward `target`
 * (halved when +$20 bit 2 is set), then apply the dead-zone nudge. Nothing
 * happens while +$26 is nonzero, +$2 bit 7 is clear and target <= 0.
 * Returns the target as possibly halved. */
int16_t steer_record_56(int16_t target);

/* Ease the current record's +$5A toward 5/8 of `target` (halved when +$20
 * bit 2 is set) by a half or, when +$62 is $14, a quarter; then apply the
 * dead-zone nudge. Returns the scaled target. */
int16_t steer_record_5a(int16_t target);

/* Whether a record is ready to pair with its partner: it is active (+$1
 * bits 6 and 0), not excluded (+$0 & $8700, +$20 bit 1), its partner (+$38,
 * when negative) is active and not excluded, and it is in state +$64 bits
 * 5-6 set with +$63 class $2x or $3x. PAIR_OVERRIDE forces "ready" once. */
int paired_record_ready(gaddr record);

/* Attitude term of the current record: 2 * |+$56| + a folded +$6A angle / 8
 * - the signed +$66 angle / 2 + (REFERENCE_18 - +$18) scaled down by
 * 2^11 or, unless +$2 bit 3, 2^13. */
int16_t attitude_term(void);

/* Record the shown record's position (+$14/+$18/+$1C) in the six-slot
 * history (twice while it is filling) and keep its +$3D history length. */
void record_position_history(void);

#endif
