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

/* $C1EBB0/$C1EC84: the high selector byte indexes 32-byte workspace records. */
gaddr workspace_record(uint16_t selector);

/* Add the selected workspace record's low-byte +6/+8 cell displacements,
 * relative to column/row, to x/z. Differences wrap as signed words before
 * conversion to fixed point. Returns the z displacement ($C1EC84). */
int32_t add_workspace_cell_steps(gaddr entry, int16_t column, int16_t row,
                                 uint32_t *x, uint32_t *z);

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

/* +$5A = +$20 when this record's +$6A exceeds 14400, -$20 when it is
 * lower but nonzero, 0 when +$6A is zero. */
void update_record_5a(gaddr record);

/* Current record: move +$26 an eighth of the way toward `target`. */
void ease_record_26(int16_t target);
/* This record's +$58 a quarter of the way toward 5/8 of `target`
 * (halved again when +$20 bit 2 is set), then kept out of the dead zone
 * ($C13C0A). Returns the target used. */
int16_t ease_record_58(gaddr record, int16_t target);

/* $C1342C: select MATRIX_SIDE_RECORD, update its three table-driven working
 * values (+$56, +$58 and +$5A), and maintain the associated status bits. */
void update_matrix_side_record(void);

/* A record's controls (+$65 above the throttle bits) from a steering
 * demand: none ($C2CAA0); roll toward `turn` ($C2CA92); a turn with rudder
 * and roll by the record's flags +$64 and bank +$6A ($C2CA26); stick back
 * or forward when `climb` passes +$56 ($C2CB86). */
/* A kind-1 record's in-sight flag (+4 bit 5): set when the viewer's +$4A
 * is $3000 or less, the target lies well ahead along the viewer's first
 * axis (its +$96 column) and the two records' first axes point the same
 * way ($C2436A). */
void update_in_sight(gaddr target, gaddr viewer);

void steer_record_neutral(gaddr record);
void steer_record_roll(gaddr record, int16_t turn);
void steer_record_turn(gaddr record, int16_t turn);
void steer_record_pitch(gaddr record, int16_t climb);

/* Mark a record pending (+$50 = -1), negating +$54 if it was not already. */
void mark_record_pending(gaddr record);

/* Point `record` at what the word at `source` + 4 selects ($C28800). A
 * negative selector picks a five-word set from VIEW_PARAMETER_TABLE by its
 * bits 8-14, stores it as the record's view and marks +$38 $FF; otherwise
 * bits 8-15 name another control record, whose view fields are copied and
 * whose index goes to +$38 with bit 7. The record is then turned toward
 * that target's x and z, and nothing happens at all when the selector is
 * empty or the named record is not active (+$0 bit 6). */
void aim_record_at_view(gaddr record, gaddr source);

/* Store view parameters into a record at +$2C..+$37. */
void set_record_view(gaddr record, int16_t a, int16_t b, int16_t c, int16_t d, uint32_t e);

/* The $C23CA6-$C23D36 prefix: for an eligible mode-eight record, find its
 * current view in the selected five-word set and advance or clear it. */
void refresh_record_view_from_table(gaddr record);

/* The linked-record view route and its $4200 easing exit ($C23FF8-$C24054).
 * Returns after that source exit; the caller chooses this route. */
void update_linked_record_view(gaddr record);

/* The $C242DE-$C24364 view lookup used when the record's high flag byte has
 * bit 4. Clears the pending byte on its return path. */
void resolve_record_zone_view(gaddr record);

/* The $C241A6-$C242DC return path after a record's view point has been
 * placed: in-sight check and table-driven status selection. */
void finish_record_view_status(gaddr record, gaddr viewer);

/* $C24056-$C240E2: gate the selected viewer, set the source flag, and mark
 * close records with selector $80. Returns zero for the zone-view route. */
int prepare_record_viewer(gaddr record, gaddr viewer, uint32_t *d4_state);

typedef struct RecordViewPointWork {
    int16_t local[3];
    int32_t world[3];
} RecordViewPointWork;

/* $C240E2-$C241A6: place a local point using the viewer's inverse matrix
 * and pack the resulting world point into the record's four words and long. */
void place_record_view_point(gaddr record, gaddr viewer, uint32_t d4_state,
                             RecordViewPointWork *work);

typedef enum RecordViewUpdateRoute {
    RECORD_VIEW_UPDATE_EARLY,
    RECORD_VIEW_UPDATE_LINKED,
    RECORD_VIEW_UPDATE_ZONE,
    RECORD_VIEW_UPDATE_PLACED,
    RECORD_VIEW_UPDATE_MODE_EIGHT
} RecordViewUpdateRoute;

typedef struct RecordViewUpdateWork {
    gaddr record, viewer;
    uint32_t d4_state;
    uint16_t dispatch_d1;
    int dispatch_valid;
    RecordViewPointWork point;
    RecordViewUpdateRoute route;
} RecordViewUpdateWork;

/* Memory-side composition of $C23CA6; the bridge replays its live registers. */
int update_record_view(gaddr record, gaddr incoming_viewer,
                       uint32_t incoming_d4, RecordViewUpdateWork *work);

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

/* Nudge *value away from zero by this record's +$6C / 128, except
 * inside the +-$500 dead zone. */
void nudge_outside_dead_zone(gaddr record, gaddr value);

/* Reset the player record's mission fields and clear records 1-3. */
void reset_mission_objects(void);
/* $C133B2: step from the current record's +$6E word (or $10 for type $3x). */
int32_t record_6e_step(void);
/* $C083E2: post message $4005, settle attempt flag, then reset objects. */
void begin_mission_reset(void);

/* Set up the player record for a new flight. */
void prepare_player_record(void);

/* Ease this record's +$56 a quarter of the way toward `target`
 * (halved when +$20 bit 2 is set), then apply the dead-zone nudge. Nothing
 * happens while +$26 is nonzero, +$2 bit 7 is clear and target <= 0.
 * Returns the target as possibly halved. */
int16_t steer_record_56(gaddr record, int16_t target);

/* Ease this record's +$5A toward 5/8 of `target` (halved when +$20
 * bit 2 is set) by a half or, when +$62 is $14, a quarter; then apply the
 * dead-zone nudge. Returns the scaled target. */
int16_t steer_record_5a(gaddr record, int16_t target);

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

/* Append a (kind, index) event, and an $FF end marker, to `events` for
 * every active record (control records: kind $10; workspace records: $40)
 * pending at cell (column, row, level); clear their pending bit. Returns the
 * advanced event pointer. */
gaddr collect_records_in_cell(int16_t column, int16_t row, int8_t level, gaddr events);

/* Accumulate a record position scaled down by `shift`: *x = (*x << 8) +
 * (+$14 & $FFFFF) >> shift, likewise *z with +$1C; *y = +$18 >> shift. */
void accumulate_record_position(gaddr record, int shift, int32_t *x, int32_t *y, int32_t *z);

/* Update the shown record +$78 toward a limit from TABLE_78_LIMIT and the
 * +$66 angle, and set +$76 from TABLE_76_TARGET. */
void update_record_76_78(void);

/* While +$66 is positive: below 400, flag the record (+$2 bit 6) once,
 * sound the alert if it is a plain player record (+$0 bits $1600 = $1000,
 * +$4 bit 1 clear), and zero +$56; otherwise ease +$56 toward -$40 or $40
 * (by the whole difference, or half when +$6C >= $6C0). */
void update_record_56_from_66(gaddr record);

/* The loop state of file_records_by_level, as its caller sees it. */
typedef struct {
    int8_t level; /* level of the last record filed ($FF: none) */
    int16_t level_offset; /* that level's list offset / 3 (level * 32) */
    int level_offset_valid; /* a filing pass selected a level this call */
    int index;    /* record index where filing stopped (16: all) */
    gaddr cursor; /* the list position last used */
} FilingState;

/* $C1D5D8: file the flagged records (+$01 bits 6 and 4) standing in cell
 * (column, row) into the 96-byte per-level lists at `lists`: each as (kind,
 * index), $FF-terminated, kind $10 for control records and $40 for
 * workspace records, clearing their bit 4. A record at the same level as
 * the previous one is appended at the same position without a scan. Stops
 * when a list is full ($5D bytes); a negative level is a fatal error. Only
 * when CELL_CHECKS is set. `state->cursor` starts as the caller's list
 * position. */
void file_records_by_level(int16_t column, int16_t row, gaddr lists, FilingState *state);

/* $C1D3F4: expand cell (column, row) of the template table at `templates`
 * into the per-level lists at `lists`, when the cell's bit is set in the
 * bitmap (a long per 32 columns, 16 bytes a row). The cell's records are
 * found by the column in its sorted word table; each is a header byte (the
 * level in bits 0-3, a pair-table index in bits 4-7) followed by entries
 * until $FF. An entry is a flag byte, split into its bit 7 and its low
 * seven bits, then either the long that follows it or the two words the
 * pair table $C1D8B6 gives. Sixteen entries a level; a seventeenth is
 * fatal error $38, and a full list ($5D bytes) ends the expansion. Each
 * level's records standing in the cell are collected as the level changes,
 * and the whole cell is then filed by level. */
void expand_cell_templates(int16_t row, int16_t column, gaddr templates, gaddr bitmap,
                           gaddr lists, gaddr cursor, FilingState *state);

/* When the record numbered STREAM_MODE (type $1x) has left its zone's box
 * (+$5D, 1-based; 0 none, a negative none), take the zone's exit for it:
 * +$7A 3 or 4 becomes 5, +$0 bit 0 clears, its view comes from the exit
 * and +$38 is set to $FF ($C28E28). Zone 0 raises error $1E. */
void check_zone_exit(void);

#endif
