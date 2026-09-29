#ifndef FA18_GAME_MESSAGES_H
#define FA18_GAME_MESSAGES_H

#include <stdint.h>

/* The cockpit message line: one entry of MESSAGE_TABLE at a time
 * ("STALL", "ALERT: IR MISSILE", ...). */

/* Choose this step's message and run its timing ($C11BFC). A posted
 * message (COCKPIT_FLAGS bit 0) holds until MESSAGE_CODE is cleared;
 * otherwise crash imminent, a posted timed message, then the threats and
 * warnings in priority order, and no message once they clear. A flashing
 * message alternates with the next entry, or with none when it also times
 * out, on the NOTIFY_CODE ticks; a timed one runs down MESSAGE_COUNTDOWN.
 * NOTIFY_CODE is consumed. */
void update_message(void);

/* Turn WARNING_CAUSES bits 9 and 14 into event bit 3 ($C33DA4). */
void take_warning_events(void);

/* Post `code` as the message, held until cleared; codes with bit 13 start
 * timed at once, $40xx and $48xx restart their timing ($C25704). */
void post_message(uint16_t code);

#endif
