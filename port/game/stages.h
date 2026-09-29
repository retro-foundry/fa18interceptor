#ifndef FA18_GAME_STAGES_H
#define FA18_GAME_STAGES_H

#include "memory.h"

/* A stage with nothing to do (several update tables point here). */
void empty_stage(void);

/* Empty the list at LIST_BUFFER. */
void reset_list(void);

/* Count a byte timer down to zero; negative timers are stopped. */
void tick_timer(gaddr timer);

/* Handlers that report "nothing" (0). */
int zero_result(void);

/* Clear the 16-long table at LONG_TABLE. */
void clear_long_table(void);
/* Fill it from `src` instead ($C11ACC). */
void load_long_table(gaddr src);

/* Depth sort: output the values of the `count` keys from largest to
 * smallest (ties keep the earlier key), skipping negative keys. */
void sort_by_depth(int16_t count);

/* Empty the message queue and reset the message sequence. */
void reset_message_sequence(void);

/* Table value for the current mode, doubled (0 for modes $7E and $7F). */
int16_t mode_offset(void);

/* Skip the stream records selected by STREAM_SKIP (52 bytes each). */
gaddr skip_stream_records(gaddr stream);

/* A display value cached in *cache (bit 15 = drawn). Returns the value to
 * draw, or -1 when nothing needs drawing. */
int16_t display_value_to_draw(gaddr cache, int16_t value);

/* Index of `key` in a sorted word table (count word, then entries), or -1
 * (after raising error $1C) when it is absent. */
int16_t find_sorted_word(gaddr table, int16_t key);

/* Stream operation $C21940: a word operand, and a skip over that many bytes
 * when the shown record's flags (+2) have bit 3 set. Returns the stream
 * position after it. */
gaddr skip_if_shown_record_flag(gaddr stream);

/* Past one more word of a face stream in STREAM_MODE $57 ($C1FED4). */
gaddr skip_word_for_mode_57(gaddr stream);

#endif
