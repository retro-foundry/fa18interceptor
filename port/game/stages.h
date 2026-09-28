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

#endif
