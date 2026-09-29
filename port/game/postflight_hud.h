#ifndef FA18_GAME_POSTFLIGHT_HUD_H
#define FA18_GAME_POSTFLIGHT_HUD_H

/* The postflight HUD display: its altitude, speed and heading tapes, the
 * projected status mark and the outer cockpit pass. */

/* $C33370. */
void draw_postflight_tape(void);

/* $C33B38. */
void draw_postflight_variant(void);

/* $C33CD2. */
void transform_postflight_record(void);

/* $C332BC. */
void draw_postflight_hud(void);

#endif
