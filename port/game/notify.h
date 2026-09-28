#ifndef FA18_GAME_NOTIFY_H
#define FA18_GAME_NOTIFY_H

/* Advance the eight-step notification cadence and set this step's code:
 * $86 when the cycle restarts, $06 at step 4, $04 at steps 2 and 6. */
void tick_notification_cadence(void);

#endif
