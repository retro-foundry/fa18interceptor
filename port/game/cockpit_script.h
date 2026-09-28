#ifndef FA18_GAME_COCKPIT_SCRIPT_H
#define FA18_GAME_COCKPIT_SCRIPT_H

#include <stdint.h>

#include "memory.h"

/* Handlers of the cockpit display script. Each takes the script position
 * and returns it advanced past the entries that do not apply to the record
 * shown (SCRIPT_RECORD). */

/* Skip 8 bytes when the record type (+$7C low nibble) is 3-6. */
gaddr skip_for_type_3_to_6(gaddr script);

/* Skip one 18-byte entry per SCRIPT_COUNT. */
gaddr skip_counted_entries(gaddr script);

/* Skip 2 bytes when the record class (+$7C bits 4-6) is below 6. */
gaddr skip_for_low_class(gaddr script);

/* Skip to the block for the record type (+$7C low nibble, 0-6). */
gaddr skip_to_type_block(gaddr script);

/* True when the viewed record is flagged (+$4 bit 6) and no context runs. */
int viewed_record_flagged(void);

#endif
